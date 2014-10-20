#include "config.h"
#include <fstream>
#include "util/exceptions.h"
#include "util/string-util.h"

namespace fract {

static Json::Value NullValue;

Config::Config(const std::string &path)
  : path_(path), directory_(RemoveFileNameFromPath(path))
{
  root_ = Load();
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

std::shared_ptr<const Json::Value> Config::Load() {
  std::string text = ReadFile(path_);
  std::shared_ptr<Json::Value> root = std::make_shared<Json::Value>();
  Json::Reader reader;
  if (!reader.parse(text, *root, false))
    throw JSONException(
      "bad JSON at " + path_ + ": " + reader.getFormattedErrorMessages());
  return root;
}

std::string Config::GetDir() const {
  return directory_;
}

Config::Version Config::Current() {
  return Version(std::atomic_load(&root_));
}

}
