#pragma once

#include "gl-common.h"
#include "texture2d.h"
#include <string>
#include <map>

namespace fract { namespace GL {

const char * const kDefaultShaderAttribnames[]={"inScreenPos","inCanvasPos"};

#define SHADER_INFO_LOG

class Shader {
public:
	Shader(std::string vert, std::string frag, int attribcnt = 2, const char * const *attribnames = kDefaultShaderAttribnames);
	~Shader();

  void SetTexture(const std::string &name, const Texture2D &texture, int unit);

	void Use();
	GLuint program_id();
	void LogUniforms(); // writes information about all active uniforms to stdout
private:
  struct Uniform {
    GLint location;
    GLint size;
    GLenum type;
  };

	GLuint vs_, ps_, program_;
  std::map<std::string, Uniform> uniforms_;
};

}}
