#pragma once

#include <functional>
#include <vector>
#include <string>

namespace fract {

// Notifies when any file from the given list changes.
// Calls all callbacks from single background thread.
// Depending on platform, may miss events in rare cases.
class FileWatcher {
 public:
  typedef std::function<void()> UpdateHandler;
  FileWatcher(
    const std::vector<std::string> &paths,
    UpdateHandler handler);
  ~FileWatcher();
 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

}
