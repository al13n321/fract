uniform vec3 CameraPos;
uniform float CameraScale;
uniform mat4 CameraRotProjInv;
uniform vec2 Resolution;
uniform vec2 ViewportOrigin;
uniform vec2 ViewportSize;

uniform float EpsilonMultiplier = 1;

in vec2 ScreenPosition;

layout (location = 0) out vec4 OutMain;
layout (location = 1) out vec4 OutNormal;
layout (location = 2) out vec4 OutColor;

void main() {
  vec2 pix = (ViewportOrigin + ViewportSize * ScreenPosition) * 2.0 - 1.0;

  vec4 dir;
  dir = CameraRotProjInv * vec4(
    pix, 0.0f, 1.0f);
  dir.xyz /= dir.w;
  dir.w = 0.0;

  vec4 a = CameraRotProjInv * vec4(
    pix - 1.0 / Resolution, 0.0f, 1.0f);
  vec4 b = CameraRotProjInv * vec4(
    pix + 1.0 / Resolution, 0.0f, 1.0f);
  a.xyz /= a.w;
  b.xyz /= b.w;
  dir.w = distance(a.xyz, b.xyz) / length(dir.xyz) * EpsilonMultiplier;
  dir.xyz = normalize(dir.xyz);

  RaytracerOutput res = TraceRay(vec4(CameraPos, 0.0), dir, CameraScale);
  
  OutMain = vec4(res.error * 4 + res.converged * 2 + res.hit,
    res.iterations, res.dist, 0.0);
  OutNormal = vec4(res.normal, 0);
  OutColor = res.color;
}

