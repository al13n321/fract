#include "debug.h"

#ifdef WIN32
#include <intrin.h>
#endif

namespace fract {

void MaybeDebugBreak() {
#ifndef NDEBUG
  #ifdef WIN32
    __debugbreak();
  #endif
#endif
}

std::atomic<int> DebugTrigger;

}
