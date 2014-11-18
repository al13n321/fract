#include "framebuffer.h"
#include <cassert>
#include "util/exceptions.h"
#include "util/string-util.h"

namespace fract { namespace GL {

Framebuffer::Framebuffer(std::initializer_list<Texture2D*> textures) {
  assert(textures.size() > 0);

  size_ = (*textures.begin())->size();

  glGenFramebuffers(1, &fbo_);CHECK_GL_ERROR();
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);CHECK_GL_ERROR();

  std::vector<GLenum> bufs;
  int i = 0;
  for (Texture2D *texture: textures) {
    glFramebufferTexture2D(
      GL_DRAW_FRAMEBUFFER,
      GL_COLOR_ATTACHMENT0 + i,
      GL_TEXTURE_2D,
      texture->name(),
      0);CHECK_GL_ERROR();
    bufs.push_back(GL_COLOR_ATTACHMENT0 + i);
    ++i;
  }

  glDrawBuffers(static_cast<int>(bufs.size()), &bufs[0]);CHECK_GL_ERROR();

  GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);CHECK_GL_ERROR();

  if (Status != GL_FRAMEBUFFER_COMPLETE)
    throw GLException("bad framebuffer status: " + ToString(Status));

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);CHECK_GL_ERROR();
}

Framebuffer::~Framebuffer() {
  if (fbo_ != 0)
    glDeleteFramebuffers(1, &fbo_);
}

void Framebuffer::BindForWriting(bool set_glviewport) {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);CHECK_GL_ERROR();
  if (set_glviewport)
    glViewport(0, 0, size_.x, size_.y);
}

void Framebuffer::Unbind() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);CHECK_GL_ERROR();
}

}}
