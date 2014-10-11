#ifdef WIN32
#include <glew.h>
#endif

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/OpenGL.h>
#endif

#include <GLFW/glfw3.h>

namespace fract { namespace GL {

// Throw if glGetError() returns non-success.
void CheckError(const char *file, int line);
#define CHECK_GL_ERROR() CheckError(__FILE__, __LINE__)

// Log OpenGL vendor, version, extensions etc.
void LogInfo();

}}
