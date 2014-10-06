#pragma once

#include "gl-common.h"
#include "util/exceptions.h"

namespace fract { namespace glfw {

class Initializer {
 public:
  Initializer() {
    if (!glfwInit())
      throw GraphicsAPIException("couldn't init glfw");
  }

  ~Initializer() {
    glfwTerminate();
  }
};

class Window {
 public:
  Window(int width, int height, const std::string &title) {
    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window)
      throw GraphicsAPIException("couldn't create window");
  }

  void MakeCurrent() {
    glfwMakeContextCurrent(window);
  }

  void GetFramebufferSize(int *width, int *height) {
    glfwGetFramebufferSize(window, width, height);
  }

  bool ShouldClose() {
    return glfwWindowShouldClose(window);
  }

  void SetShouldClose() {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  void SwapBuffers() {
    glfwSwapBuffers(window);
  }

  void SetKeyCallback(GLFWkeyfun callback) {
    glfwSetKeyCallback(window, callback);
  }

  void SetScrollCallback(GLFWscrollfun callback) {
    glfwSetScrollCallback(window, callback);
  }

  void SetMouseButtonCallback(GLFWmousebuttonfun callback) {
    glfwSetMouseButtonCallback(window, callback);
  }

  void SetCursorPosCallback(GLFWcursorposfun callback) {
    glfwSetCursorPosCallback(window, callback);
  }

  bool IsKeyPressed(int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
  }

  void GetCursorPos(double *x, double *y) {
    glfwGetCursorPos(window, x, y);
  }

  void SetCursorPos(double x, double y) {
    glfwSetCursorPos(window, x, y);
  }

  void SetInputMode(int mode, int value) {
    glfwSetInputMode(window, mode, value);
  }

  void SetTitle(const std::string &title) {
    glfwSetWindowTitle(window, title.c_str());
  }

  ~Window() {
    glfwDestroyWindow(window);
  }
 private:
  GLFWwindow *window;
};

}}
