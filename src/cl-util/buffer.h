#pragma once

#include "context.h"
#include "event.h"

namespace fract { namespace CL {

class Buffer {
 public:
  Buffer(cl_mem_flags flags, int size, Context &context);
  ~Buffer();

  Buffer(const Buffer &rhs) = delete;
  Buffer& operator=(const Buffer &rhs) = delete;

  // please don't modify contents of const CLBuffer
  cl_mem buffer() const { return buffer_; }
  int size() const { return size_; }

 private:
  int size_;
  cl_mem buffer_;
};
    
}}
