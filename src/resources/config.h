#pragma once

#include <string>
#include <set>
#include <json/json.h>
#include "util/file-watcher.h"

namespace fract {

// Reads and preprocesses a json config from file.
// Reloads it when file changes, finds what nodes are different and
// notifies subscribers.
// All callbacks are made sequentially - under a single mutex.
// In case of error when loading config keeps previous version.
// If there's no previous version, keeps empty tree.
// Contains directory where the config file is located under key "_dir".
class Config {
 public:
  // Has the same thread safety as shared_ptr.
  class Version {
   public:
    Version(std::shared_ptr<const Json::Value> root);
    // Allow copy and assign.

    const Json::Value& GetRoot();

    // nullValue if doesn't exist. Makes a deep copy.
    const Json::Value& TryGet(const std::vector<std::string> &path);

    // Exception if doesn't exist. Makes a deep copy.
    // Examples:
    // Get({"renderer"})
    // Get({"camera", "pos", "x"})
    const Json::Value& Get(const std::vector<std::string> &path);

    // Exception if doesn't exist or is not a string.
    std::string GetString(const std::vector<std::string> &path);
   private:
    std::shared_ptr<const Json::Value> root_;
  };

  typedef std::function<void(Version conf)> UpdateHandler;

 private:
  struct HandlerInfo {
    UpdateHandler handler;
    uint64_t id;

    HandlerInfo() {}
    HandlerInfo(UpdateHandler handler, uint64_t id): handler(handler), id(id) {}

    bool operator<(const HandlerInfo &rhs) const {
      return id < rhs.id;
    }
  };

  typedef std::multimap<std::vector<std::string>, HandlerInfo> Subscriptions;

public:
  class Subscription {
   public:
    ~Subscription() {
      std::lock_guard<std::mutex> lock(config_.mutex_);
      for (auto it: iterators_)
        config_.subscriptions_.erase(it);
    }
    Subscription(const Subscription &rhs) = delete;
    Subscription& operator=(const Subscription &rhs) = delete;
   private:
    friend class Config;
    Subscription(Config &config): config_(config) {}
    Config &config_;
    std::vector<Subscriptions::iterator> iterators_;
  };

  typedef std::unique_ptr<Subscription> SubscriptionPtr;

  // Doesn't load anything synchronously. You should either use subscriptions
  // or get Current() every time you need a value (e.g. each frame).
  Config(const std::string &path);

  Config(const Config &rhs) = delete;
  Config& operator=(const Config &rhs) = delete;

  Version Current();

  // Subscribe to changes to listed paths and their subpaths.
  // Also calls the handler during or right afer this call, even if there were
  // no changes or config is not yet loaded.
  // (If you don't need this behavior, you can add a parameter to disable it.)
  // The subsription is active until the returned Subscription is destroyed.
  // The Subscription must be destroyed before the Config.
  SubscriptionPtr Subscribe(
    std::initializer_list<std::vector<std::string>> paths,
    UpdateHandler handler);

  // TODO: Can add method to switch to different file.
 private:
  std::string path_;
  std::string directory_;
  std::shared_ptr<const Json::Value> root_;

  Subscriptions subscriptions_;
  uint64_t handler_id_increment_{};

  // For
  //  - subscriptions_
  //  - calling handlers
  //  - loading json
  // (note: not root_; Current() must not wait for loading and handlers)
  std::mutex mutex_;

  std::unique_ptr<FileWatcher> file_watcher_;

  std::shared_ptr<const Json::Value> Load();
  bool Diff(
    const Json::Value &old_subtree,
    const Json::Value &new_subtree,
    std::vector<std::string> &path,
    std::set<HandlerInfo> &out_handlers);
  void Update();
};

typedef std::shared_ptr<Config> ConfigPtr;

}
