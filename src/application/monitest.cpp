#include <memory>
#include <iostream>
#include "gl-util/glfw-util.h"
using namespace fract;

// Investigating strange behavior when there's a fullscreen window on rift
// and non-fullscreen window on a normal display.

#define CHECK { int err = glGetError(); if (err) { std::cerr << "error " << err << " on line " << __LINE__ << std::endl; }}

int main() {
  glfw::Initializer glfw_init;

  std::unique_ptr<glfw::Window> win(new glfw::Window(ivec2(500, 500), "window"));
  win->MakeCurrent();
  GL::InitGl3wIfNeeded();

  std::unique_ptr<glfw::Window> fs(new glfw::Window(ivec2(1920, 1080), "fullscreen", 1));

  int frame = 0;
  while (!win->ShouldClose()) {
    if (rand()%2000==0)
      ++frame;

    fs->MakeCurrent();

    glViewport(0, 0, 500, 500); CHECK;
    glClearColor((float)(frame % 2), (float)(frame % 4 / 2), (float)(frame % 8 / 4), 1); CHECK;
    glClear(GL_COLOR_BUFFER_BIT); CHECK;

    fs->SwapBuffers();

    win->MakeCurrent();

    glViewport(0, 0, 500, 500); CHECK;
    ++frame;
    glClearColor((float)(frame % 2), (float)(frame % 4 / 2), (float)(frame % 8 / 4), 1); CHECK;
    --frame;
    glClear(GL_COLOR_BUFFER_BIT); CHECK;

    win->SwapBuffers();

    glfwPollEvents();
  }

  return 0;
}
