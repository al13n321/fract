#pragma once

#include "texture2d.h"
#include <initializer_list>

namespace fract { namespace GL {

// Allows rendering to textures. Textures should outlive this object.
class Framebuffer {
 public:
  // Adds textures as color attachments to the FBO.
  Framebuffer(std::initializer_list<Texture2D*> textures);
  ~Framebuffer();

  void BindForWriting();
  static void Unbind();
 private:
  GLuint fbo_ = 0;
};

}}
