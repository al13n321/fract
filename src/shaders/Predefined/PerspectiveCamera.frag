uniform vec3 CameraPos;
uniform float CameraScale;
uniform mat4 CameraRotProjInv;

in vec2 ScreenPosition;

layout (location = 0) out vec4 OutMain;
layout (location = 1) out vec4 OutNormal;
layout (location = 2) out vec4 OutColor;

void main() {
  // TODO: unproject, call TraceRay, put result to MRT
  OutMain = vec4(ScreenPosition,1,1);
  OutNormal = vec4(1,1,1,1);
  OutColor = vec4(1,1,1,1);
}

