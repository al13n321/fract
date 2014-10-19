#include "shader.h"
#include <string>
#include <iostream>
#include <fstream>
#include "util/string-util.h"
#include "util/exceptions.h"
#include "util/logging.h"
using namespace std;

namespace fract { namespace GL {

static void CompileShader(
  GLint shader, const string &path, const string &text
) {
  int str_l = static_cast<int>(text.length());
  const char *c_str = text.c_str();
  glShaderSource(shader, 1, &c_str, &str_l);CHECK_GL_ERROR();
  glCompileShader(shader);CHECK_GL_ERROR();
  GLint ret;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ret);CHECK_GL_ERROR();

  const int maxlen = 100000;
  GLchar buffer[maxlen];
  GLsizei l;
  glGetShaderInfoLog(shader, maxlen, &l, buffer);CHECK_GL_ERROR();

  if (!ret || buffer[0] != '\0') {
    if (ret) {
      cerr << "compiled: " << path << "\n";
    } else {
      cerr << "compilation failed: " << path << "\n";
      cerr << "preprocessed shader saved to "
           << DumpWithLineNumbers(text) << "\n";
    }
    cerr << buffer << "\n" << endl;

    if (!ret)
      throw ShaderCompilationException("compilation error in " + path);
  }
}

Shader::Shader(
  const std::string &vert_path,
  const std::string &frag_path,
  const std::string &vert_text,
  const std::string &frag_text,
  int attribcnt, const char * const * attribnames
) {
  GLint ret;

  vs_ = glCreateShader(GL_VERTEX_SHADER);CHECK_GL_ERROR();
  CompileShader(vs_, vert_path, vert_text);

  ps_ = glCreateShader(GL_FRAGMENT_SHADER);CHECK_GL_ERROR();
  CompileShader(ps_, frag_path, frag_text);

  program_ = glCreateProgram();
  glAttachShader(program_, vs_);CHECK_GL_ERROR();
  glAttachShader(program_, ps_);CHECK_GL_ERROR();
  for (int i = 0; i < attribcnt; ++i) {
    glBindAttribLocation(program_, i, attribnames[i]);CHECK_GL_ERROR();
  }
  glLinkProgram(program_);CHECK_GL_ERROR();

  glGetProgramiv(program_, GL_LINK_STATUS, &ret);CHECK_GL_ERROR();

  const int maxlen = 100000;
  GLchar buffer[maxlen];
  GLsizei l;
  glGetProgramInfoLog(program_, maxlen, &l, buffer);CHECK_GL_ERROR();
  if (!ret || buffer[0] != '\0'){
    if (ret)
      cerr << "linked: " << vert_path << " and " << frag_path << "\n";
    else
      cerr << "finking failed: " << vert_path << " and " << frag_path << "\n";
    cerr << buffer << "\n" << endl;

    if (!ret)
      throw ShaderCompilationException(
        "linking error in " + vert_path + " and " + frag_path);
  }

  int cnt;
  glGetProgramiv(program_, GL_ACTIVE_UNIFORMS, &cnt);CHECK_GL_ERROR();
  for (int i = 0; i < cnt; ++i) {
    char name[GL_ACTIVE_UNIFORM_MAX_LENGTH];
    GLsizei namelen;
    Uniform uni;
    uni.location = i;
    glGetActiveUniform(
      program_, i, GL_ACTIVE_UNIFORM_MAX_LENGTH,
      &namelen, &uni.size, &uni.type, name);CHECK_GL_ERROR();
    uniforms_[name] = uni;
  }
}
void Shader::Use() {
  glUseProgram(program_);CHECK_GL_ERROR();
}
GLuint Shader::program_id() {
  return program_;
}
void Shader::LogUniforms() {
  cerr << uniforms_.size() << " active uniforms:" << endl;
  for (const auto it: uniforms_) {
    cerr << it.first << " size: " << it.second.size
      << ", type: " << it.second.type << endl;
  }
  cerr << endl;
}

void Shader::SetTexture(
  const std::string &name, const Texture2D &texture, int unit
) {
  if (uniforms_.count(name))
    texture.AssignToUniform(uniforms_[name].location, unit);
}

Shader::~Shader() {
  glDetachShader(program_, vs_);
  glDetachShader(program_, ps_);
  glDeleteShader(vs_);
  glDeleteShader(ps_);
  glDeleteProgram(program_);
}

}}
