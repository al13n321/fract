#include "texture2d.h"
#include <iostream>
using namespace std;

namespace fract { namespace GL {

Texture2D::Texture2D(ivec2 size, GLint internalFormat, GLint filter)
		: size_(size) {
	unsigned int *data;
	data = (unsigned int*) new GLuint[
		((size_.x * size_.y) * 4 * sizeof(unsigned int))];
	memset(data, 0, ((size_.x * size_.y) * 4 * sizeof(unsigned int)));

	glGenTextures(1, &name_);
	glBindTexture(GL_TEXTURE_2D, name_);
	glTexImage2D(
		GL_TEXTURE_2D, 0, internalFormat, size_.x, size_.y, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	delete [] data;
}
Texture2D::Texture2D(
		ivec2 size, GLint internalFormat, GLenum format, GLenum type,
		const GLvoid *data, GLint filter)
		: size_(size) {
	glGenTextures(1, &name_);
	glBindTexture(GL_TEXTURE_2D, name_);
	glTexImage2D(
		GL_TEXTURE_2D, 0, internalFormat, size_.x, size_.y, 0,	format, type, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}
void Texture2D::AssignToUniform(int uniform, int unit) const {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, name_);
	glUniform1i(uniform, unit);
}
void Texture2D::SetFilter(GLint filter) {
	glBindTexture(GL_TEXTURE_2D, name_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
}

void Texture2D::SetPixels(ivec2 pos, ivec2 size, GLenum format, GLenum type, void *data) {
	glBindTexture(GL_TEXTURE_2D, name_);
	glTexSubImage2D(
		GL_TEXTURE_2D, 0, pos.x, pos.y, size.x, size.y, format, type, data);
}

void Texture2D::SetPixels(GLenum format, GLenum type, void *data) {
	SetPixels(ivec2(0, 0), size_, format, type, data);
}

Texture2D::~Texture2D() {
	glDeleteTextures(1, &name_);
}

}}
