#include <CoreServices/CoreServices.h>
#include <iostream>

void Callback(
  ConstFSEventStreamRef streamRef,
  void *clientCallBackInfo,
  size_t numEvents,
  void *eventPaths,
  const FSEventStreamEventFlags eventFlags[],
  const FSEventStreamEventId eventIds[]
) {
  std::cout << "info: " << (uint64_t)clientCallBackInfo << '\n'
            << "numEvents: " << numEvents << '\n'
            << "eventPaths[0]: " << (numEvents ? (((char**)eventPaths)[0]) : "") << '\n'
            << "eventFlags[0]: " << (numEvents ? eventFlags[0] : 0u) << '\n'
            << "eventIds[0]: " << (numEvents ? eventIds[0] : 0ull) << '\n'
            << std::endl;
}

int main(int argc, char **argv) {
  CFStringRef mypath = CFStringCreateWithCString(
    NULL, argv[1], kCFStringEncodingUTF8);
  CFArrayRef pathsToWatch = CFArrayCreate(
    NULL, (const void **)&mypath, 1, NULL);
  FSEventStreamContext context {
    .version = 0,
    .info = (void*) 42,
    .retain = NULL,
    .release = NULL,
    .copyDescription = NULL};

  /* Create the stream, passing in a callback */
  FSEventStreamRef stream = FSEventStreamCreate(
    NULL,
    &Callback,
    &context,
    pathsToWatch,
    kFSEventStreamEventIdSinceNow,
    0,
    kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagWatchRoot
  );

  FSEventStreamScheduleWithRunLoop(
    stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
  FSEventStreamStart(stream);

  CFRunLoopRun();

  FSEventStreamRelease(stream);
  CFRelease(pathsToWatch);
  CFRelease(mypath);

  return 0;
}
