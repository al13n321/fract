#include "command-queue.h"
#include "util/exceptions.h"

namespace fract { namespace CL {

CommandQueue::CommandQueue(Context &context) {
  cl_command_queue_properties queue_properties = 0;

  queue_properties |= CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;

#ifdef PROFILING_ENABLED
  queue_properties |= CL_QUEUE_PROFILING_ENABLE;
#endif
  queue_ = clCreateCommandQueue(context.context(), context.device_id(), queue_properties, NULL);
  if (!queue_)
    throw CLException("failed to create command queue");
}

CommandQueue::~CommandQueue() {
  clFinish(queue_);
  clReleaseCommandQueue(queue_);
}

void CommandQueue::WaitForAll() {
  int err = clFinish(queue_);
  if (err != CL_SUCCESS)
    throw CLException("failed to clFinish");
}

void CommandQueue::RunKernel2D(Kernel &kernel, int size0, int size1,
  const EventList *wait_list, Event *out_event
) {
  int s0 = 1;
  while (s0 < size0 && s0 * s0 < kernel.work_group_size()) // something nearly square
      s0 *= 2;
  s0 = std::min(s0, size0);
  int s1 = std::min(kernel.work_group_size() / s0, size1);

  size_t local[2] = { static_cast<size_t>(s0), static_cast<size_t>(s1) };
  size_t global[2] = {
    static_cast<size_t>((size0 + s0 - 1) / s0 * s0),
    static_cast<size_t>((size1 + s1 - 1) / s1 * s1)};

  int wait_list_size = 0;
  const cl_event *wait_list_events = NULL;

  if (wait_list) {
    wait_list_size = wait_list->size();
    if (wait_list_size)
      wait_list_events = wait_list->events();
  }

  cl_event event;
  cl_event *eventptr = out_event ? &event : NULL;

  int err = clEnqueueNDRangeKernel(
    queue_, kernel.kernel(), 2, NULL, global, local,
    wait_list_size, wait_list_events, eventptr);
  if (err != CL_SUCCESS)
    throw CLException("failed to enqueue kernel");

  if (out_event)
    out_event->reset(event);
}

}}
