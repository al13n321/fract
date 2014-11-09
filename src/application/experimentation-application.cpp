#include <iostream>
#include "raytracer/raytracing-engine.h"
#include "raytracer/renderer.h"
#include "gl-util/glfw-util.h"
#include "util/camera.h"
#include "util/stopwatch.h"
#include "util/string-util.h"

using namespace fract;

int winhei = 512;
int winwid = 512;
const int imgwid = 512;
const int imghei = 512;

bool mouse_pressed;
double initial_mousex;
double initial_mousey;
double prev_mousex;
double prev_mousey;

float movement_speed = 0.5f; // units per second at scale 1
float looking_speed = 0.4f; // degrees per pixel
double scaling_speed = 1.1; // coefficient per scroll wheel unit

Stopwatch fps_stopwatch;
const int fps_update_period_frames = 10;
const double fps_update_period_seconds = .5;
int cur_frame_number;
int last_fps_update_frame;

std::unique_ptr<glfw::Initializer> glfw_init;

// Window must be destroyed after everything that can make OpenGL calls.
std::unique_ptr<glfw::Window> window;

Camera camera;

ConfigPtr config;

std::unique_ptr<RaytracingEngine> raytracer;
std::unique_ptr<Renderer> renderer;

static void LogGLFWError(int code, const char *message) {
  std::cerr << "glfw error " << code << ": " << message << std::endl;
}

static void KeyCallback(
  GLFWwindow* w, int key, int scancode, int action, int mods
) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      window->SetShouldClose();
    } else if (key == GLFW_KEY_C) {
      camera.FromJson(config->Current().TryGet({"camera"}));
    }
  }
}

static void ScrollCallback(GLFWwindow *w, double dx, double dy) {
  if (dy != 0) {
    camera.set_scale(camera.scale() * pow(scaling_speed, dy));
  }
}

static void MouseButtonCallback(
  GLFWwindow *w, int button, int action, int mods
) {
  if (button == GLFW_MOUSE_BUTTON_1) {
    if (action == GLFW_PRESS) {
      mouse_pressed = true;
      window->GetCursorPos(&initial_mousex, &initial_mousey);
      // NOTE: this causes glitches with low FPS; look for a better way
      window->SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      prev_mousex = -1;
    } else {
      mouse_pressed = false;
      window->SetCursorPos(initial_mousex, initial_mousey);
      window->SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
  }
}

static void CursorPosCallback(GLFWwindow *w, double x, double y) {
  if (mouse_pressed) {
    double dx = x - prev_mousex;
    double dy = y - prev_mousey;
    if (prev_mousex != -1 && (dx != 0 || dy != 0)) {
      camera.set_yaw(camera.yaw() + static_cast<float>(dx * looking_speed));
      camera.set_pitch(camera.pitch() - static_cast<float>(dy * looking_speed));
    }
    prev_mousex = x;
    prev_mousey = y;
  }
}

static void ProcessKeyboardInput(double frame_time) {
  fvec3 movement(0, 0, 0);
  if (window->IsKeyPressed(GLFW_KEY_A))
    movement.x -= 1;
  if (window->IsKeyPressed(GLFW_KEY_D))
    movement.x += 1;
  if (window->IsKeyPressed(GLFW_KEY_W))
    movement.z -= 1;
  if (window->IsKeyPressed(GLFW_KEY_S))
    movement.z += 1;

  if (!movement.IsZero()) {
    camera.MoveRelative(movement * static_cast<float>(frame_time) * movement_speed);
  }
}

static void UpdateFPS() {
  if (cur_frame_number - last_fps_update_frame >= fps_update_period_frames ||
      fps_stopwatch.TimeSinceRestart() > fps_update_period_seconds) {
    double fps = (cur_frame_number - last_fps_update_frame)
      / fps_stopwatch.Restart();
    last_fps_update_frame = cur_frame_number;
    window->SetTitle("FPS: " + ToString(fps));
  }
}

int main(int argc, char **argv) {
  try {
    freopen("log.txt", "w", stderr);
    srand(static_cast<unsigned int>(time(nullptr)));

    std::string config_path;
    if (argc == 1)
      config_path = "shaders/conf.json";
    else if (argc == 2)
      config_path = argv[1];
    else
      throw CommandLineArgumentsException(
        "0 or 1 command line arguments expected");

    config = std::make_shared<Config>(config_path);

    glfwSetErrorCallback(&LogGLFWError);
    glfw_init.reset(new glfw::Initializer());
    window.reset(new glfw::Window(winwid, winhei, "upchk"));

    window->MakeCurrent();
    window->SwapInterval(1);
    window->GetFramebufferSize(&winwid, &winhei);
    window->SetPosition(20, 40);

    if (gl3wInit())
      throw GLException("failed to initialize gl3w");

    GL::LogInfo();

    camera.set_aspect_ratio(static_cast<float>(winwid) / winhei);
    camera.set_position(fvec3(0, 0, 10));

    auto camera_subscription =
      config->Subscribe({{"camera"}}, [&](Config::Version conf) {
        auto json = conf.TryGet({ "camera" });
        if (!json.isNull())
          camera.FromJson(json);
      }, Config::SYNC);

    raytracer.reset(new RaytracingEngine(imgwid, imghei, config));
    renderer.reset(new Renderer(config));

    window->SetKeyCallback(&KeyCallback);
    window->SetScrollCallback(&ScrollCallback);
    window->SetMouseButtonCallback(&MouseButtonCallback);
    window->SetCursorPosCallback(&CursorPosCallback);

    Stopwatch frame_stopwatch;

    while (!window->ShouldClose()) {
      double frame_time = frame_stopwatch.Restart();
      ++cur_frame_number;

      config->PollUpdates();

      ProcessKeyboardInput(frame_time);
      UpdateFPS();

      const RaytracedView &raytraced =
        raytracer->Raytrace(
          camera.position(), camera.scale(), camera.RotationProjectionMatrix());
      
      glViewport(0, 0, winwid, winhei);CHECK_GL_ERROR();
      renderer->Render(raytraced, winwid, winhei);

      window->SwapBuffers();
      glfwPollEvents();
    }
  } catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << std::endl;
    return 2;
  }

  // Workaround for MSVC bug: https://connect.microsoft.com/VisualStudio/feedback/details/747145
  renderer.reset();
  raytracer.reset();
  config.reset();

  return 0;
}
