#include "buffer.h"
#include <cassert>
#include "util/exceptions.h"

namespace fract { namespace CL {

Buffer::Buffer(cl_mem_flags flags, int size, Context &context) {
  assert(size > 0);
  size_ = size;
  buffer_ = clCreateBuffer(context.context(), flags, size, NULL, NULL);
  if (!buffer_)
    throw CLException("failed to create a buffer");
}

Buffer::~Buffer() {
  clReleaseMemObject(buffer_);
}

}}
