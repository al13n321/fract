#include "file-watcher.h"
#include <thread>
#include "util/exceptions.h"

#ifdef WIN32
#error FileWatcher is not implemented for windows.\
       Go ahead and do it, it should be easy!
#else
#include <sys/stat.h>
#endif

#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

namespace fract {

#ifdef WIN32
#error add implementation here

#elif __APPLE__

// Mac OS implementation with File System Events. Might send duplicate updates.
// Shouldn't miss updates. Though I'm sure I observed it consistently missing
// updates once, but was never able to reproduce.

class CFString {
 public:
  CFString(const std::string &str) {
    ref_ = CFStringCreateWithCString(
      nullptr, str.c_str(), kCFStringEncodingUTF8);
    if (!ref_)
      throw AppleException("can't create CFString from string " + str);
  }
  CFString(const CFString &rhs) = delete;
  CFString(CFString &&rhs) = default;
  CFString& operator=(const CFString &rhs) = delete;
  ~CFString() {
    if (ref_)
      CFRelease(ref_);
  }

  CFStringRef Get() { return ref_; }
 private:
  CFStringRef ref_ {};
};

class CFArrayString {
 public:
  CFArrayString(const std::vector<std::string> &strings) {
    for (size_t i = 0; i < strings.size(); ++i) {
      cf_strings_.emplace_back(strings[i]);
    }
    ref_ = CFArrayCreate(
      nullptr,
      // Relying on (trivial) CFString data layout.
      reinterpret_cast<const void **>(&cf_strings_[0]),
      cf_strings_.size(),
      nullptr);
    if (!ref_)
      throw AppleException("can't create CFArray of strings");
  }
  CFArrayString(const CFArrayString &rhs) = delete;
  CFArrayString& operator=(const CFArrayString &rhs) = delete;
  ~CFArrayString() {
    if (ref_)
      CFRelease(ref_);
  }

  CFArrayRef Get() { return ref_; }
 private:
  std::vector<CFString> cf_strings_;
  CFArrayRef ref_ {};
};

struct FileWatcher::Impl {
  UpdateHandler handler_;
  std::vector<std::string> paths_;
  std::thread thread_;

  bool initialized_ {};
  std::mutex mutex_;
  std::condition_variable cv_;

  CFRunLoopRef run_loop_ {};
  std::exception_ptr exception_;

  Impl(
    const std::vector<std::string> &paths,
    UpdateHandler handler): handler_(handler), paths_(paths)
  {
    thread_ = std::thread(std::bind(&FileWatcher::Impl::ThreadFunc, this));

    {
      std::unique_lock<std::mutex> lk(mutex_);
      cv_.wait(lk, [&]{ return initialized_; });
    }
    if (exception_) {
      thread_.join();
      std::rethrow_exception(exception_);
    }
  }

  ~Impl() {
    if (run_loop_) {
      CFRunLoopStop(run_loop_);
      thread_.join();
    }
  }

  static void Callback(
    ConstFSEventStreamRef streamRef,
    void *clientCallBackInfo,
    size_t numEvents,
    void *eventPaths,
    const FSEventStreamEventFlags eventFlags[],
    const FSEventStreamEventId eventIds[]
  ) {
    try {
      Impl *impl = reinterpret_cast<Impl*>(clientCallBackInfo);
      impl->handler_();
    } catch (...) {
      LogCurrentException();
    }
  }

  void ThreadFunc() {
    CFRunLoopRef run_loop = CFRunLoopGetCurrent();
    std::unique_ptr<CFArrayString> cf_paths;

    try {
      cf_paths.reset(new CFArrayString(paths_));
    } catch (...) {
      {
        std::lock_guard<std::mutex> lk(mutex_);
        exception_ = std::current_exception();
        initialized_ = true;
      }
      cv_.notify_one();
      return;
    }

    FSEventStreamContext context {
      .version = 0,
      .info = this,
      .retain = NULL,
      .release = NULL,
      .copyDescription = NULL};

    FSEventStreamRef stream = FSEventStreamCreate(
      NULL,
      &FileWatcher::Impl::Callback,
      &context,
      cf_paths->Get(),
      kFSEventStreamEventIdSinceNow,
      0,
      kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagWatchRoot);

    FSEventStreamScheduleWithRunLoop(
      stream, run_loop, kCFRunLoopDefaultMode);
    FSEventStreamStart(stream);

    {
      std::lock_guard<std::mutex> lk(mutex_);
      run_loop_ = run_loop;
      initialized_ = true;
    }
    cv_.notify_one();

    CFRunLoopRun();

    FSEventStreamRelease(stream);
  }
};

#else // neither WIN32 nor __APPLE__

// A fallback implementation that just checks timestamp each second.
// Note that it may miss updates if they hapen within timestamp resolution unit.

struct FileWatcher::Impl {
  UpdateHandler handler_;
  std::vector<std::string> paths_;
  std::map<std::string, uint64_t> modification_times_;
  std::thread thread_;
  
  bool shutdown_flag_;
  std::mutex shutdown_mutex_;
  std::condition_variable shutdown_cv_;

  static uint64_t GetModificationTime(const std::string &path) {
    struct stat s {};
    stat(path.c_str(), &s);
    return s.st_mtimespec.tv_sec * 1000_000_000ull + s.st_mtimespec.tv_nsec;
  }

  Impl(
    const std::vector<std::string> &paths,
    UpdateHandler handler): handler_(handler)
  {
    UpdateAll(false);
    thread_ = std::thread(std::bind(FileWatcher::Impl::ThreadFunc, this));
  }

  ~Impl() {
    {
        std::lock_guard<std::mutex> lk(shutdown_mutex_);
        shutdown_flag_ = true;
    }
    shutdown_cv_.notify_one();
    thread_.join();
  }

  void UpdateAll(bool notify) {
    for (auto &it: files_) {
      uint64_t t = GetModificationTime(it.first);
      if (t != it.second.modification_time) {
        it.second.modification_time = t;
        if (notify) {
          notify = false;
          hander_();
        }
      }
    }
  }

  void ThreadFunc() {
    while (true) {
      {
        std::unique_lock<std::mutex> lk(shutdown_mutex_);
        shutdown_cv_.wait_for(lk, std::chrono::seconds(1));
        if (shutdown_flag_)
          break;
      }

      UpdateAll(true);
    }
  }
};

#endif

}
