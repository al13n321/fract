#pragma once

#include "gl-util/texture2d.h"
#include "context.h"
#include "buffer.h"
#include "command-queue.h"

namespace fract { namespace CL {

class Texture2DRef {
public:
  Texture2DRef(std::shared_ptr<GL::Texture2D> texture, cl_mem_flags flags,
    std::shared_ptr<Context> context);
  ~Texture2DRef();

  Texture2DRef(const Texture2DRef &rhs) = delete;
  Texture2DRef& operator=(const Texture2DRef &rhs) = delete;

  // write-only
  // uchar4 RGBA
  // only valid between BeginUpdates and EndUpdates
  cl_mem buffer() { return buffer_; }

  void BeginUpdates(CommandQueue &queue);
  void EndUpdates(CommandQueue &queue);

  std::shared_ptr<GL::Texture2D> texture() { return texture_; }

  void CopyFrom(const Buffer &buffer, bool blocking, CommandQueue &queue);
private:
  std::shared_ptr<Context> context_;
  cl_mem buffer_;
  std::shared_ptr<GL::Texture2D> texture_;
  GLuint pbo_;
};
    
}}
