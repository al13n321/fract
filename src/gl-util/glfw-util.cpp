#include "glfw-util.h"

#ifdef WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

namespace fract { namespace glfw {

HWND Window::GetHWND() {
  return glfwGetWin32Window(window);
}

void Window::Focus() {
#ifdef WIN32
  SetFocus(GetHWND());
#else
  std::cerr
    << "Window::Focus() is not implemented for this platform. "
    << "Consider implementing it to avoid the crappy user experience "
    << "you are currently having." << std::endl;
#endif
}

}}