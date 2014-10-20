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

  class Listener {
   public:
    ~Listener();
   private:

  };

  Config(const std::string &path);

  Config(const Config &rhs) = delete;
  Config& operator=(const Config &rhs) = delete;

  // Directory in which the config file is located.
  std::string GetDir() const;

  Version Current();

  // Subscribe to changes to listed paths and their subpaths.
  Listener Subscribe(
    std::initializer_list<std::vector<std::string>> paths,
    UpdateHandler handler);
 private:
  std::string path_;
  std::string directory_;
  std::shared_ptr<const Json::Value> root_;

  std::shared_ptr<const Json::Value> Load();
};

typedef std::shared_ptr<Config> ConfigPtr;

}
