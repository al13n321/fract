#pragma once

#include <atomic>

namespace fract {

// In debug build, try to trigger breakpoint.
void MaybeDebugBreak();

// "'" key increments it.
// Someone may check it to trigger some actions used for debugging.
extern std::atomic<int> DebugTrigger;

}
