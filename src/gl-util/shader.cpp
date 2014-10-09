#include "shader.h"
#include <string>
#include <iostream>
#include <fstream>
#include "util/string-util.h"
#include "util/exceptions.h"
using namespace std;

namespace fract { namespace GL {

static void CompileShader(GLint shader, const string &path) {
  string str = ReadFile(path);
  int str_l = static_cast<int>(str.length());
  const char *c_str = str.c_str();
  glShaderSource(shader, 1, &c_str, &str_l);
  glCompileShader(shader);
  GLint ret;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ret);

  const int maxlen = 100000;
  GLchar buffer[maxlen];
  GLsizei l;
  glGetShaderInfoLog(shader, maxlen, &l, buffer);

  if (!ret || buffer[0] != '\0') {
    if (ret)
      cerr << "compiled: " << path << "\n";
    else
      cerr << "compilation failed: " << path << "\n";
    cerr << buffer << "\n" << endl;

    if (!ret)
      throw ShaderCompilationException("compilation error in " + path);
  }
}

Shader::Shader(string vert, string frag, int attribcnt, const char * const * attribnames) {
  GLint ret;

  vs_ = glCreateShader(GL_VERTEX_SHADER);
  CompileShader(vs_, vert);

  ps_ = glCreateShader(GL_FRAGMENT_SHADER);
  CompileShader(ps_, frag);

  program_ = glCreateProgram();
  glAttachShader(program_, vs_);
  glAttachShader(program_, ps_);
  for (int i = 0; i < attribcnt; ++i)
    glBindAttribLocation(program_, i, attribnames[i]);
  glLinkProgram(program_);

  glGetProgramiv(program_, GL_LINK_STATUS, &ret);

  const int maxlen = 100000;
  GLchar buffer[maxlen];
  GLsizei l;
  glGetProgramInfoLog(program_, maxlen, &l, buffer);
  if (!ret || buffer[0] != '\0'){
    if (ret)
      cerr << "linked: " << vert << " and " << frag << "\n";
    else
      cerr << "finking failed: " << vert << " and " << frag << "\n";
    cerr << buffer << "\n" << endl;

    if (!ret)
      throw ShaderCompilationException(
        "linking error in " + vert + " and " + frag);
  }
}
GLint Shader::GetUniformLocation(const std::string &name) {
  return glGetUniformLocation(program_, name.c_str());
}
void Shader::Use() {
  glUseProgram(program_);
}
GLuint Shader::program_id() {
  return program_;
}
void Shader::LogUniforms() {
  int cnt;
  glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &cnt);
  cerr << cnt << " active uniforms:" << endl;
  for (int i = 0; i < cnt; ++i) {
    char name[GL_ACTIVE_UNIFORM_MAX_LENGTH];
    GLsizei namelen;
    GLint size;
    GLenum type;
    glGetActiveUniform(program_, i, GL_ACTIVE_UNIFORM_MAX_LENGTH, &namelen, &size, &type, name);
    cerr << name << " size: " << size << ", type: " << type << endl;
  }
  cerr << endl;
}
Shader::~Shader() {
  glDetachShader(program_, vs_);
  glDetachShader(program_, ps_);
  glDeleteShader(vs_);
  glDeleteShader(ps_);
  glDeleteProgram(program_);
}

}}
