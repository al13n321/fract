#include "framebuffer.h"
#include "util/exceptions.h"
#include "util/string-util.h"

namespace fract { namespace GL {

Framebuffer::Framebuffer(std::initializer_list<Texture2D*> textures) {
  glGenFramebuffers(1, &fbo_);CHECK_GL_ERROR();
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);CHECK_GL_ERROR();

  {
    int i = 0;
    for (Texture2D *texture: textures) {
      glFramebufferTexture2D(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0 + i,
        GL_TEXTURE_2D,
        texture->name(),
        0);CHECK_GL_ERROR();
      ++i;
    }
  }

  GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);CHECK_GL_ERROR();

  if (Status != GL_FRAMEBUFFER_COMPLETE)
    throw GLException("bad framebuffer status: " + ToString(Status));

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);CHECK_GL_ERROR();
}

Framebuffer::~Framebuffer() {
  if (fbo_ != 0)
    glDeleteFramebuffers(1, &fbo_);
}

void Framebuffer::BindForWriting() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_);CHECK_GL_ERROR();
}

void Framebuffer::Unbind() {
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);CHECK_GL_ERROR();
}

}}
