#pragma once

#include "context.h"
#include "kernel.h"
#include "event.h"

namespace fract { namespace CL {

class CommandQueue {
 public:
  CommandQueue(Context &context);
  ~CommandQueue();

  CommandQueue(const CommandQueue &rhs) = delete;
  CommandQueue& operator=(const CommandQueue &rhs) = delete;

  cl_command_queue queue() { return queue_; }

  void WaitForAll();

  void RunKernel2D(Kernel &kernel, int size0, int size1,
    const EventList *wait_list, Event *out_event);
 private:
  cl_command_queue queue_;
};

}}
