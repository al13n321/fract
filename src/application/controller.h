#pragma once

#include "gl-util/glfw-util.h"

namespace fract {

// Coordinates raytracing and rendering.
// Ok, this is too abstract. The point is having two subclasses:
// for rendering to a normal window and to Oculus Rift.
// This interface captures what is common for these two cases.
// Owns window (including OpenGL context), render targets,
// Raytracer and Renderer. (and, in future, LookupCube).
class Controller {
 public:
  virtual ~Controller() {};

  // Makes OpenGL context current for this thread.
  virtual void MakeCurrent() = 0;

  // Give it a chance to save some state. E.g. update camera with last HMD pose.
  virtual void WillBecomeNonCurrent() {}

  virtual void Render() = 0;

  virtual glfw::Window* GetWindow() = 0;
};

}