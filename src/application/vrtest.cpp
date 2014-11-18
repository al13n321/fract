#include "vr/ovr-util.h"
#include "gl-util/glfw-util.h"
#include "gl-util/framebuffer.h"

using namespace fract;

// Almost minimal code that renders something on Rift.

int main() {
  ovr::Initializer ovr_init;
  ovr::HMD hmd;

  ivec2 texsize[2] = {
    hmd.GetTextureSize(ovrEye_Left, 1.f),
    hmd.GetTextureSize(ovrEye_Left, 1.f)};
  
  glfw::Initializer glfw_init;
  glfw::Window window(hmd.GetResolution(), "vrtest");
  window.SetPosition(hmd.GetWindowPos());
  window.MakeCurrent();

  if (gl3wInit())
    throw GLException("failed to initialize gl3w");

  hmd.StartTracking();
  hmd.ConfigureRendering();

  GL::Texture2D tex[2] = {
    GL::Texture2D(texsize[0], GL_RGB8, GL_LINEAR),
    GL::Texture2D(texsize[1], GL_RGB8, GL_LINEAR),
  };
  GL::Framebuffer fb[2] = {
    GL::Framebuffer({&tex[0]}),
    GL::Framebuffer({&tex[1]})};

  while (!window.ShouldClose()) {
    hmd.BeginFrame();

    ovrPosef pose[2];
    hmd.GetEyePoses({&pose[0], &pose[1]});

    fb[0].BindForWriting();
    glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    fb[1].BindForWriting();
    glClearColor(0,1,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    GL::Framebuffer::Unbind();
    glViewport(0, 0, hmd.GetResolution().x, hmd.GetResolution().y);CHECK_GL_ERROR();

    hmd.EndFrame({&tex[0], &tex[1]});

    glfwPollEvents();
  }

	return 0;
}
