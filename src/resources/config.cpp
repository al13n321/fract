#include "config.h"
#include <fstream>
#include "util/exceptions.h"
#include "util/string-util.h"

namespace fract {

static Json::Value NullValue;

Config::Config(const std::string &path)
  : path_(path), directory_(RemoveFileNameFromPath(path))
{
  auto root = std::make_shared<Json::Value>(Json::objectValue);
  (*root)["_dir"] = Json::Value(directory_);
  root_ = root;

  file_watcher_.reset(new FileWatcher(
    {path_}, std::bind(&Config::Update, this)));
}

Config::Version::Version(std::shared_ptr<const Json::Value> root)
  : root_(root) {}

const Json::Value& Config::Version::GetRoot() {
  return *root_;
}

const Json::Value& Config::Version::TryGet(
  const std::vector<std::string> &path
) {
  const Json::Value *value = root_.get();
  for (const std::string &name: path) {
    if (!value->isObject())
      return NullValue;
    value = &(*value)[name];
  }
  return *value;
}
const Json::Value& Config::Version::Get(const std::vector<std::string> &path) {
  const Json::Value &res = TryGet(path);
  if (res.isNull())
    throw NoConfigKeyException("no key " + ToString(path));
  return res;
}
std::string Config::Version::GetString(const std::vector<std::string> &path) {
  const Json::Value &value = Get(path);
  if (!value.isString())
    throw ConfigValueFormatException(
      "value for key " + ToString(path) + " is not a string");
  return value.asString();
}

Config::Version Config::Current() {
  return Version(std::atomic_load(&root_));
}

std::shared_ptr<const Json::Value> Config::Load() {
  std::string text = ReadFile(path_);
  std::shared_ptr<Json::Value> root = std::make_shared<Json::Value>();
  Json::Reader reader;
  if (!reader.parse(text, *root, false))
    throw JSONException(
      "bad JSON at " + path_ + ": " + reader.getFormattedErrorMessages());
  if (!root->isObject())
    throw JSONException("root JSON node of a config must be an object");
  (*root)["_dir"] = Json::Value(directory_);
  return root;
}

bool Config::Diff(
    const Json::Value &old_subtree,
    const Json::Value &new_subtree,
    std::vector<std::string> &path,
    std::set<HandlerInfo> &out_handlers) {
  bool diff = false;
  if (!old_subtree.isObject() && !new_subtree.isObject()) {
    diff = old_subtree != new_subtree;
  } else {
    if (old_subtree.isObject() != new_subtree.isObject())
      diff = true;
    std::vector<std::string> all_keys;
    if (old_subtree.isObject()) {
      std::vector<std::string> keys = old_subtree.getMemberNames();
      all_keys.insert(all_keys.end(), keys.begin(), keys.end());
    }
    if (new_subtree.isObject()) {
      std::vector<std::string> keys = new_subtree.getMemberNames();
      all_keys.insert(all_keys.end(), keys.begin(), keys.end());
    }
    std::sort(all_keys.begin(), all_keys.end());
    all_keys.erase(std::unique(all_keys.begin(), all_keys.end()),
      all_keys.end());
    for (const std::string &key: all_keys) {
      path.push_back(key);
      diff |= Diff(
        old_subtree.isObject() ? old_subtree[key] : Json::Value(),
        new_subtree.isObject() ? new_subtree[key] : Json::Value(),
        path, out_handlers);
      path.pop_back();
    }
  }

  if (diff) {
    auto range = subscriptions_.equal_range(path);
    for (auto it = range.first; it != range.second; ++it) {
      out_handlers.insert(it->second);
    }
  }

  return diff;
}

void Config::Update() {
  std::lock_guard<std::mutex> lock(mutex_);
  std::shared_ptr<const Json::Value> old_root = std::atomic_load(&root_);
  std::shared_ptr<const Json::Value> new_root = Load();
  std::atomic_store(&root_, new_root);
  std::set<HandlerInfo> handlers;
  std::vector<std::string> temp_path;
  Diff(*old_root, *new_root, temp_path, handlers);
  for (auto &handler: handlers)
    handler.handler(Version(new_root));
}

Config::SubscriptionPtr Config::Subscribe(
    std::initializer_list<std::vector<std::string>> paths,
    UpdateHandler handler) {
  std::lock_guard<std::mutex> lock(mutex_);
  SubscriptionPtr subscription(new Subscription(*this));
  for (const auto &path: paths) {
    subscription->iterators_.push_back(
      subscriptions_.insert(std::make_pair(
        path, HandlerInfo(handler, handler_id_increment_++))));
  }
  handler(Current());
  return subscription;
}

}
