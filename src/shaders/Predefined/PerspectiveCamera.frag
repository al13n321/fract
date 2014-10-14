uniform vec3 CameraPos;
uniform float CameraScale;
uniform mat4 CameraRotProjInv;

in vec2 ScreenPosition;

void main() {
  // TODO: unproject, call TraceRay, put result to MRT
  gl_FragColor = vec4(1,1,1,1);
}

