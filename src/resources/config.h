#pragma once

#include <string>
#include <json/json.h>

namespace fract {

// Reads and preprocesses a json config from file.
// Reloads it when file changes, finds what nodes are different and
// notifies subscribers.
// All callbacks are made sequentially from single background thread.
class Config {
 public:
  // Has the same thread safety as shared_ptr.
  class Version {
   public:
    Version(std::shared_ptr<const Json::Value> root);
    // Allow copy and assign.

    const Json::Value& GetRoot();

    // nullValue if doesn't exist. Makes a deep copy.
    const Json::Value& TryGet(std::initializer_list<std::string> path);
    const Json::Value& TryGet(const std::vector<std::string> &path);

    // Exception if doesn't exist. Makes a deep copy.
    // Examples:
    // Get({"renderer"})
    // Get({"camera", "pos", "x"})
    // (actually don't do the last one, get whole camera at once and then
    //  access its fields to avoid race conditions when reloading)
    const Json::Value& Get(std::initializer_list<std::string> path);
    const Json::Value& Get(const std::vector<std::string> &path);

    // Exception if doesn't exist or not string.
    std::string GetString(std::initializer_list<std::string> path);
    std::string GetString(const std::vector<std::string> &path);
   private:
    std::shared_ptr<const Json::Value> root_;
  };

  Config(const std::string &path);

  Config(const Config &rhs) = delete;
  Config& operator=(const Config &rhs) = delete;

  // Directory in which the config file is located.
  std::string GetDir() const;

  Version Current();

  // TODO: subscriptions and reloading. Callbacks should receive a Version.
 private:
  std::string path_;
  std::string directory_;
  Json::Reader reader_;
  std::shared_ptr<const Json::Value> root_;
};

typedef std::shared_ptr<Config> ConfigPtr;

}
