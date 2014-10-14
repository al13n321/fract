#version 410

in vec4 inScreenPos;
in vec2 inCanvasPos;
out vec2 ScreenPosition;

void main(void){
	gl_Position = inScreenPos;
	ScreenPosition=inCanvasPos;
}