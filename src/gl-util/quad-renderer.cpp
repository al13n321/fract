#include "quad-renderer.h"
#include <cstdlib>
#include <memory>
#include <iostream>

namespace fract { namespace GL {

QuadRenderer* QuadRenderer::defaultInstance(){
  static std::unique_ptr<QuadRenderer> shared_quad_(new QuadRenderer());
  return shared_quad_.get();
}

void QuadRenderer::init(float x, float y, float wid, float hei, float cx1, float cy1, float cx2, float cy2) {
  GLfloat vertices[16] = {
                    x*2 - 1, y*2 - 1         , 0, 1,
                    x*2 - 1, (y + hei)*2 - 1 , 0, 1,
            (x + wid)*2 - 1, (y + hei)*2 - 1 , 0, 1,
            (x + wid)*2 - 1, y*2 - 1         , 0, 1};
  GLfloat positions[16] = {
          cx1, cy1, 0, 0,
          cx1, cy2, 0, 0,
          cx2, cy2, 0, 0,
          cx2, cy1, 0, 0};

  glGenVertexArrays(1, &vao_);CHECK_GL_ERROR();
  glBindVertexArray(vao_);CHECK_GL_ERROR();

  glGenBuffers(2, vbo_);CHECK_GL_ERROR();

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);CHECK_GL_ERROR();
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    CHECK_GL_ERROR();
  glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);CHECK_GL_ERROR();
  glEnableVertexAttribArray(0);CHECK_GL_ERROR();

  glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);CHECK_GL_ERROR();
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);CHECK_GL_ERROR();
  glVertexAttribPointer((GLuint)1, 4, GL_FLOAT, GL_FALSE, 0, 0);CHECK_GL_ERROR();
  glEnableVertexAttribArray(1);CHECK_GL_ERROR();

  glBindVertexArray(0);CHECK_GL_ERROR();
}
QuadRenderer::QuadRenderer(float x, float y, float wid, float hei, float cx1, float cy1, float cx2, float cy2) {
  init(x, y, wid, hei, cx1, cy1, cx2, cy2);
}
QuadRenderer::QuadRenderer(float x, float y, float wid, float hei, bool normOrigin, bool normScale) {
  float cx1 = x, cy1 = y;
  if (normOrigin)
    cx1 = cy1 = 0;
  float cx2 = cx1 + wid, cy2 = cy1 + hei;
  if (normScale) {
    cx2 = cx1 + 1;
    cy2 = cy1 + 1;
  }
  init(x, y, wid, hei, cx1, cy1, cx2, cy2);
}
QuadRenderer::QuadRenderer() {
  init(0, 0, 1, 1, 0, 0, 1, 1);
}
void QuadRenderer::Render() {
  glBindVertexArray(vao_);CHECK_GL_ERROR();

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);CHECK_GL_ERROR();

  glBindVertexArray(0);CHECK_GL_ERROR();
}
QuadRenderer::~QuadRenderer() {
  glBindBuffer(GL_ARRAY_BUFFER, 0);CHECK_GL_ERROR();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);CHECK_GL_ERROR();
  glDeleteBuffers(2, vbo_);CHECK_GL_ERROR();

  glBindVertexArray(0);CHECK_GL_ERROR();
  glDeleteVertexArrays(1, &vao_);CHECK_GL_ERROR();
}

}}
