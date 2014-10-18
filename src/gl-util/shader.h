#pragma once

#include "gl-common.h"
#include <string>

namespace fract { namespace GL {

const char * const kDefaultShaderAttribnames[]={"inScreenPos","inCanvasPos"};

#define SHADER_INFO_LOG

class Shader {
public:
	Shader(std::string vert, std::string frag, int attribcnt = 2, const char * const *attribnames = kDefaultShaderAttribnames);
	~Shader();

  // TODO: methods to set uniforms by name instead. Memoizing name->location.
  GLint GetUniformLocation(const std::string &name);

	void Use();
	GLuint program_id();
	void LogUniforms(); // writes information about all active uniforms to stdout
private:
	GLuint vs_, ps_, program_;
};

}}
