#ifdef WIN32
#define NOMINMAX
#include <glew.h>
#include <wglew.h>
#include <glut.h>
#include <CL/opencl.h>
#undef NOMINMAX
#endif

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenCL/OpenCL.h>
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#undef glBindVertexArray
#undef glDeleteVertexArrays
#undef glGenVertexArrays
#undef glIsVertexArray
#define glBindVertexArray glBindVertexArrayAPPLE
#define glDeleteVertexArrays glDeleteVertexArraysAPPLE
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glIsVertexArray glIsVertexArrayAPPLE
#endif
