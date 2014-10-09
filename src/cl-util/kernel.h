#pragma once

#include "context.h"
#include "buffer.h"
#include "util/mat.h"

namespace fract { namespace CL {

class Kernel {
 public:
  Kernel(std::string file_name, std::string params,
    std::string kernel_name, std::shared_ptr<Context> context);

  // file_name relative to include_dir
  Kernel(std::string include_dir, std::string file_name, std::string params,
    std::string kernel_name, std::shared_ptr<Context> context);

  ~Kernel();

  Kernel(const Kernel &rhs) = delete;
  Kernel& operator=(const Kernel &rhs) = delete;

  int work_group_size() const;
  cl_kernel kernel() const;

  void SetArg(int index, int size, void *data);
  // equivalent to SetArg(index, size, NULL)
  void SetLocalBufferArg(int index, int size);
  void SetIntArg(int index, int val);
  void SetFloat4Arg(int index, fvec3 xyz, float w);
  void SetFloatArg(int index, float val);
  // please don't modify (in kernel) contents of const CLBuffer
  void SetBufferArg(int index, const Buffer *buf);
  void SetFloat16Arg(int index, fmat4 mat);
 private:
  std::shared_ptr<Context> context_;
  cl_program program_;
  cl_kernel kernel_;
  int work_group_size_;
    
  void Init(std::string text, std::string params, std::string kernel_name,
    std::shared_ptr<Context> context);
};
    
}}
