#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#endif

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace fract { namespace GL {

// Throw if glGetError() returns non-success.
void ThrowIfError(const char *file, int line);

// Returns true if glError() returns non-success.
bool LogIfError(const char *file, int line);

#define CHECK_GL_ERROR() fract::GL::ThrowIfError(__FILE__, __LINE__)
#define SOFT_CHECK_GL_ERROR() fract::GL::LogIfError(__FILE__, __LINE__)

// Log OpenGL vendor, version, extensions etc.
void LogInfo();

}}
