#include "gl-common.h"
#include <iostream>
#include <map>
#include "util/exceptions.h"
#include "util/string-util.h"

namespace fract { namespace GL {

void CheckError(const char *file, int line) {
  static std::map<GLenum, std::string> names = {
    {GL_NO_ERROR, "GL_NO_ERROR. No error has been recorded. The value of this symbolic constant is guaranteed to be 0."},
    {GL_INVALID_ENUM, "GL_INVALID_ENUM. An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag."},
    {GL_INVALID_VALUE, "GL_INVALID_VALUE. A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag."},
    {GL_INVALID_OPERATION, "GL_INVALID_OPERATION. The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag."},
    {GL_INVALID_FRAMEBUFFER_OPERATION, "GL_INVALID_FRAMEBUFFER_OPERATION. The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag."},
    {GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY. There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded."},
    {GL_STACK_UNDERFLOW, "GL_STACK_UNDERFLOW. An attempt has been made to perform an operation that would cause an internal stack to underflow."},
    {GL_STACK_OVERFLOW, "GL_STACK_OVERFLOW. An attempt has been made to perform an operation that would cause an internal stack to overflow."},
  };

  GLenum err = glGetError();
  if (err != GL_NO_ERROR)
    throw GLException(std::string("GL error at ")
      + file + ":" + ToString(line) + " " + names[err]);
}

void LogInfo() {
  std::cerr << "GL vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cerr << "GL renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cerr << "GL version: " << glGetString(GL_VERSION) << std::endl;
  std::cerr << "GLSL: "
    << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cerr << "GL extensions: " << glGetString(GL_EXTENSIONS) << std::endl;
  std::cerr << std::endl;
}

}}
