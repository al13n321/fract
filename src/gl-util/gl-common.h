#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#endif

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

namespace fract { namespace GL {

// Throw if glGetError() returns non-success.
void ThrowIfError(const char *file, int line);

// Returns true if glError() returns non-success.
bool LogIfError(const char *file, int line);

#define CHECK_GL_ERROR() ThrowIfError(__FILE__, __LINE__)
#define SOFT_CHECK_GL_ERROR() LogIfError(__FILE__, __LINE__)

// Log OpenGL vendor, version, extensions etc.
void LogInfo();

}}
