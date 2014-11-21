#include <iostream>
#include "normal-controller.h"
#include "gl-util/glfw-util.h"
#include "resources/camera.h"
#include "util/stopwatch.h"
#include "util/string-util.h"

#ifdef USE_OVR
#include "vr/ovr-util.h"
#include "ovr-controller.h"
#endif

using namespace fract;

bool mouse_pressed;
double initial_mousex;
double initial_mousey;
double prev_mousex;
double prev_mousey;

Stopwatch fps_stopwatch;
const int fps_update_period_frames = 10;
const double fps_update_period_seconds = .5;
int cur_frame_number;
int last_fps_update_frame;

std::unique_ptr<glfw::Initializer> glfw_init;

std::unique_ptr<Config> config;
Config::ContextPtr config_context;

std::unique_ptr<Camera> camera;

std::vector<std::unique_ptr<Controller>> controllers;

// Current controller and its window.
Controller *controller;
glfw::Window *window;
size_t controller_idx;

static void LogGLFWError(int code, const char *message) {
  std::cerr << "glfw error " << code << ": " << message << std::endl;
}

static void KeyCallback(
  GLFWwindow* w, int key, int scancode, int action, int mods);
static void ScrollCallback(GLFWwindow *w, double dx, double dy);
static void MouseButtonCallback(
  GLFWwindow *w, int button, int action, int mods);
static void CursorPosCallback(GLFWwindow *w, double x, double y);

static void DeactivateController() {
  window->SetScrollCallback(nullptr);
  window->SetMouseButtonCallback(nullptr);
  window->SetCursorPosCallback(nullptr);
  // (leaving KeyCallback)

  window->SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  prev_mousex = -1;
  if (mouse_pressed) {
    mouse_pressed = false;
    window->SetCursorPos(initial_mousex, initial_mousey);
  }

  window->SetTitle("(inactive)");

  controller->Deactivate();
}

static void ActivateController() {
  controller = controllers[controller_idx].get();
  controller->Activate();
  window = controller->GetWindow();

  window->SetKeyCallback(&KeyCallback);
  window->SetScrollCallback(&ScrollCallback);
  window->SetMouseButtonCallback(
    &MouseButtonCallback);
  window->SetCursorPosCallback(&CursorPosCallback);

  if (controller->CaptureMouse())
    window->SetInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  window->Focus();
}

static void KeyCallback(
  GLFWwindow* w, int key, int scancode, int action, int mods
) {
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      window->SetShouldClose();
    } else if (key == GLFW_KEY_C) {
      camera->Reset();
    } else if (key == GLFW_KEY_ENTER) {
      if (controllers.size() > 1) {
        DeactivateController();
        controller_idx = (controller_idx + 1) % controllers.size();
        ActivateController();
      }
    }
  }
}

static void ScrollCallback(GLFWwindow *w, double dx, double dy) {
  if (dy != 0) {
    camera->ScaleRelative(dy);
  }
}

static void MouseButtonCallback(
  GLFWwindow *w, int button, int action, int mods
) {
  if (button == GLFW_MOUSE_BUTTON_1 && !controller->CaptureMouse()) {
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
  if (mouse_pressed || controller->CaptureMouse()) {
    double dx = x - prev_mousex;
    double dy = y - prev_mousey;
    if (prev_mousex != -1) {
      camera->TurnRelative(fvec2((float)dx, (float)-dy));
      //                                           ^
      //                                           |
      // Convert down-facing window-space Y to up-facing OpenGL screen-space Y.
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
    camera->MoveRelative(movement * static_cast<float>(frame_time));
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
#ifdef USE_OVR
  std::unique_ptr<ovr::Initializer> ovr_init;
#endif

  try {
    freopen("log.txt", "w", stderr);
    srand(static_cast<unsigned int>(time(nullptr)));

    std::string config_path = "shaders/conf.json";
    bool no_vr = false;
    if (argc > 3)
      throw CommandLineArgumentsException(
        "0 - 2 command line arguments expected");
    if (argc > 1)
      config_path = argv[1];
    if (argc > 2)
      if (argv[2] == std::string("no-vr"))
        no_vr = true;
      else
        throw CommandLineArgumentsException(
          "second argument should be 'no-vr'");
      

    config.reset(new Config(config_path));
    config_context = config->NewContext();

#ifdef USE_OVR
    if (!no_vr)
      ovr_init.reset(new ovr::Initializer());
#endif

    glfwSetErrorCallback(&LogGLFWError);
    glfw_init.reset(new glfw::Initializer());

    camera.reset(new Camera(config_context.get()));

#ifdef USE_OVR
    if (!no_vr)
      controllers.emplace_back(new OVRController(config_context.get(), camera.get()));
#endif

    controllers.emplace_back(new NormalController(
      config_context.get(), camera.get()));
    controller_idx = controllers.size() - 1;
    ActivateController();

    GL::LogInfo();

    Stopwatch frame_stopwatch;

    while (true) {
      bool should_close = false;
      for (auto &c: controllers)
        should_close |= c->GetWindow()->ShouldClose();
      if (should_close)
        break;

      double frame_time = frame_stopwatch.Restart();
      ++cur_frame_number;

      config_context->PollUpdates();

      ProcessKeyboardInput(frame_time);
      UpdateFPS();

      controller->Render();

      glfwPollEvents();
    }
  } catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << std::endl;

    // Workaround for MSVC bug:
    // https://connect.microsoft.com/VisualStudio/feedback/details/747145
    controllers.clear();
    camera.reset();
    config_context.reset();
    config.reset();

    return 2;
  }

  // Same as above.
  controllers.clear();
  camera.reset();
  config_context.reset();
  config.reset();

  return 0;
}
