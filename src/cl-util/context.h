#pragma once

#include "cl-common.h"

namespace fract { namespace CL {

class ContextHolder {
 public:
  ContextHolder();
  ContextHolder(cl_context context);
  ~ContextHolder();

  void reset(cl_context context);

  cl_context get() { return context_; }
 private:
  cl_context context_;
};

// OpenCL context and preferred device ID. Multiple devices not supported.
class Context {
 public:
  Context();

  cl_context context() { return context_.get(); }
  cl_device_id device_id() { return device_id_; }

  Context(const Context &rhs) = delete;
  Context& operator=(const Context &rhs) = delete;
 private:
  ContextHolder context_;
  cl_device_id device_id_;
};

}}
