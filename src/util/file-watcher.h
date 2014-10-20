#pragma once

namespace fract {

// Notifies when any file from the given list changes.
// Calls all callbacks from single background thread.
class FileWatcher {
 public:
  typedef std::function<void()> UpdateHandler;
  FileWatcher(
    const std::vector<std::string> &paths,
    UpdateHandler handler);
  ~FileWatcher();
 private:
  std::thread thread_;
};

}
