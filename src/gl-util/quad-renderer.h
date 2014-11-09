#pragma once

#include "gl-common.h"

namespace fract { namespace GL {

class QuadRenderer{
public:
	QuadRenderer(); // covers viewport
	QuadRenderer(float x, float y, float wid, float hei, bool normOrigin, bool normScale); // covers region
	QuadRenderer(float x, float y, float wid, float hei, float cx1, float cy1, float cx2, float cy2); // explicit texture coordinates
	~QuadRenderer();

	void Render();
private:
	void init(float x, float y, float wid, float hei ,float cx1, float cy1, float cx2, float cy2);

	GLuint vao_, vbo_[2];
};

}}
