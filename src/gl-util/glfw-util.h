#pragma once

#include "gl-common.h"
#include "util/exceptions.h"

namespace fract { namespace glfw {

class Initializer {
 public:
  Initializer() {
    if (!glfwInit())
      throw GLException("couldn't init glfw");
  }

  ~Initializer() {
    glfwTerminate();
  }
};

class Window {
 public:
  Window(int width, int height, const std::string &title,
    bool resizable = false
  ) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, resizable ? GL_TRUE : GL_FALSE);
    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window)
      throw GLException("couldn't create window");
  }

  void MakeCurrent() {
    glfwMakeContextCurrent(window);
  }

  void GetFramebufferSize(int *width, int *height) {
    glfwGetFramebufferSize(window, width, height);
  }

  bool ShouldClose() {
    return !!glfwWindowShouldClose(window);
  }

  void SetShouldClose() {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  void SwapInterval(int interval) {
    glfwSwapInterval(interval);
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

  void SetPosition(int x, int y) {
    glfwSetWindowPos(window, x, y);
  }

  ~Window() {
    glfwDestroyWindow(window);
  }
 private:
  GLFWwindow *window;
};

}}
