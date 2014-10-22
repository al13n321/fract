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
  // Also calls the handler inside or right after construction, even if
  // there were no changes to the file and even if it doesn't exist.
  // (If you don't need this behavior, you can add a parameter to disable it.)
  FileWatcher(
    const std::vector<std::string> &paths,
    UpdateHandler handler);
  ~FileWatcher();
 private:
  struct Impl;
  std::unique_ptr<Impl> impl_;
};

}
