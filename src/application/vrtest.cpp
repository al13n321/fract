#include <memory>
#include "vr/ovr-util.h"
#include "gl-util/glfw-util.h"
#include "gl-util/framebuffer.h"

using namespace fract;

// Almost minimal code that renders something on Rift.

int main() {
  ovr::Initializer ovr_init;
  ovr::HMD hmd;

  glfw::Initializer glfw_init;
  glfw::Window window(hmd.GetResolution(), "vrtest");
  window.SetPosition(hmd.GetWindowPos());
  window.MakeCurrent();

  if (gl3wInit())
    throw GLException("failed to initialize gl3w");

  hmd.StartTracking();
  hmd.ConfigureRendering(&window, false);

  ivec2 texsize[2];
  std::unique_ptr<GL::Texture2D> tex[2];
  std::unique_ptr<GL::Framebuffer> fb[2];

  for (int i = 0; i < 2; ++i) {
    texsize[i] = hmd.GetTextureSize(i, 1.f),
    tex[i].reset(new GL::Texture2D(texsize[i], GL_RGB8, GL_LINEAR));
    fb[i].reset(new GL::Framebuffer({tex[i].get()}));
  }

  int frame = 0;
  while (!window.ShouldClose()) {
    hmd.BeginFrame();

    ovrPosef pose[2];
    hmd.GetEyePoses({&pose[0], &pose[1]});

    fb[frame%2]->BindForWriting();
    glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    if (frame<1000 || rand()%10==0)
      ++frame;

    fb[frame%2]->BindForWriting();
    glClearColor(0,1,0,1);
    glClear(GL_COLOR_BUFFER_BIT);

    GL::Framebuffer::Unbind();
    glViewport(0, 0, hmd.GetResolution().x, hmd.GetResolution().y);CHECK_GL_ERROR();

    hmd.EndFrame({tex[0].get(), tex[1].get()});

    glfwPollEvents();
  }

	return 0;
}
