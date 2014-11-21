#pragma once

#include <string>
#include <set>
#include <mutex>
#include <json/json.h>
#include "util/file-watcher.h"
#include "util/vec.h"

namespace fract {

// Reads and preprocesses a json config from file.
// Reloads it when file changes, finds what nodes are different and
// notifies subscribers.
// In case of error when loading config keeps previous version.
// If there's no previous version, keeps empty tree.
// Contains directory where the config file is located under key "_dir".
class Config {
 public:
  enum HandlerSyncMode {
    SYNC_NOW, // Called inside PollUpdates() and inside Subscribe().
              // For cases when you need handler to be called on main thread,
              // and when initialization is no different from update
              // (e.g. compiling a shader).
    SYNC,     // Called inside PollUpdates().
              // For cases when initialization is different from update
              // (e.g. creating and resizing window)
              // Subscribe before initialization to not
              // risk missing first update.
    ASYNC,    // Called from background thread and inside Subscribe().
              // (NOTE: ASYNC_NOT_NOW would be racy)
  };

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

    // Exception if exists but not a string.
    std::string GetString(
      const std::vector<std::string> &path,
      const std::string &default_value);
   private:
    std::shared_ptr<const Json::Value> root_;
  };

  typedef std::function<void(Version conf)> UpdateHandler;

  class View;

 private:
  struct HandlerInfo {
    UpdateHandler handler;
    View *view; // nullptr for async.
    uint64_t id;

    HandlerInfo() {}
    HandlerInfo(UpdateHandler handler, View *view, uint64_t id):
      handler(handler), view(view), id(id) {}

    bool operator<(const HandlerInfo &rhs) const {
      return id < rhs.id;
    }
  };

  typedef std::multimap<std::vector<std::string>, HandlerInfo> Subscriptions;

public:
  class Subscription {
   public:
    ~Subscription() {
      {
        std::lock_guard<std::mutex> lock(config_.mutex_);
        for (auto it : iterators_)
          config_.subscriptions_.erase(it);
      }

      if (handler_.view) {
        std::lock_guard<std::mutex> lock(handler_.view->mutex_);
        handler_.view->pending_handlers_.erase(handler_);
      }
    }
    Subscription(const Subscription &rhs) = delete;
    Subscription& operator=(const Subscription &rhs) = delete;
   private:
    friend class Config;
    friend class Config::View;
    Subscription(Config &config): config_(config) {}
    Config &config_;
    std::vector<Subscriptions::iterator> iterators_;
    HandlerInfo handler_;
  };

  typedef std::unique_ptr<Subscription> SubscriptionPtr;

  // Allows to subscribe to config updates. Doesn't own Config.
  // The reason for not just having Subscribe() method in Config is that some
  // handlers should only be called from a specific thread and with
  // specific OpenGL context.
  // Doing PollUpdates() for different View's in different contexts allows it.
  class View {
   public:
    View(const View &rhs) = delete;
    View& operator=(const View &rhs) = delete;

    Config* Config() { return config_; }
    Version Current();

    // Subscribe to changes to listed paths and their subpaths.
    // Unless sync=SYNC, also calls the handler during this call, even if there
    // were no changes or config is not yet loaded.
    // ASYNC callbacks are made sequentially - under a single mutex.
    // The subsription is active until the returned Subscription is destroyed.
    // The Subscription must be destroyed before the View.
    SubscriptionPtr Subscribe(
      std::initializer_list<std::vector<std::string>> paths,
      UpdateHandler handler, HandlerSyncMode sync = ASYNC);

   protected:
    friend class Config;
    friend class Config::Subscription;

    fract::Config *config_;
    std::mutex mutex_;
    std::set<HandlerInfo> pending_handlers_;

    View(fract::Config *config) : config_(config) {}
  };

  class Context : public View {
   public:
    Context(const Context &rhs) = delete;
    Context& operator=(const Context &rhs) = delete;

    // Calls sync handlers for updates that happened since the last call.
    void PollUpdates();

   private:
    friend class Config;
    Context(fract::Config *config) : View(config) {}
  };

  typedef std::unique_ptr<Context> ContextPtr;

  // Doesn't load anything synchronously. You should either use subscriptions
  // or get Current() every time you need a value (e.g. each frame).
  Config(const std::string &path);

  Config(const Config &rhs) = delete;
  Config& operator=(const Config &rhs) = delete;

  Version Current();
  ContextPtr NewContext();
 private:
  std::string path_;
  std::string directory_;
  std::shared_ptr<const Json::Value> root_;

  Subscriptions subscriptions_;
  uint64_t handler_id_increment_{};

  // For
  //  - subscriptions_
  //  - calling handlers
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

  // Catches and logs all exceptions.
  static void CallHandler(UpdateHandler handler, Version version);
};

}
