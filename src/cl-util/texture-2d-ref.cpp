#include "texture-2d-ref.h"
#include <cassert>
#include "util/exceptions.h"
using namespace std;

namespace fract { namespace CL {

Texture2DRef::Texture2DRef(std::shared_ptr<GL::Texture2D> texture,
  cl_mem_flags flags, std::shared_ptr<Context> context
) {
  assert(texture);
  assert(context);
  context_ = context;
  texture_ = texture;

  glGenBuffers(1, &pbo_);
  glBindBuffer(GL_ARRAY_BUFFER, pbo_);

  uint32_t size = texture->width() * texture->height() * 4;
  glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  int err;
  buffer_ = clCreateFromGLBuffer(
    context_->context(), CL_MEM_WRITE_ONLY, pbo_, &err);
  if (err != CL_SUCCESS)
  	throw CLException("failed to create CL buffer from PBO");
}

Texture2DRef::~Texture2DRef() {
  clReleaseMemObject(buffer_);
  glDeleteBuffers(1, &pbo_);
}

void Texture2DRef::BeginUpdates(CommandQueue &queue) {
  int err = clEnqueueAcquireGLObjects(
    queue.queue(), 1, &buffer_, 0, NULL, NULL);
  if (err != CL_SUCCESS)
  	throw CLException("failed to acquire GL object");
}

void Texture2DRef::EndUpdates(CommandQueue &queue) {
  int err = clEnqueueReleaseGLObjects(
    queue.queue(), 1, &buffer_, 0, NULL, NULL);
  if (err != CL_SUCCESS)
  	throw CLException("failed to release GL object");

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_);
  glBindTexture(GL_TEXTURE_2D, texture_->name());

  glTexSubImage2D(
    GL_TEXTURE_2D, 0, 0, 0, texture_->width(), texture_->height(),
    GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void Texture2DRef::CopyFrom(
  const Buffer &buffer, bool blocking, CommandQueue &queue
) {
  BeginUpdates(queue);

  int err = clEnqueueCopyBuffer(
    queue.queue(), buffer.buffer(), buffer_, 0, 0,
    texture_->width() * texture_->height() * 4, 0, NULL, 0);

  if(err != CL_SUCCESS)
    throw CLException("failed to copy buffer pbo buffer");

  EndUpdates(queue);
    
  if (blocking)
    queue.WaitForAll();
}
    
}}
