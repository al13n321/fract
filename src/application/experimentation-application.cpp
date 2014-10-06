#include <iostream>
#include "raytracer/raytracing-engine.h"
#include "raytracer/renderer.h"
#include "cpu-raytracers/test-raytracers.h"
#include "gl-util/glfw-util.h"
#include "util/camera.h"
#include "util/stopwatch.h"

using namespace fract;

int winhei = 512;
int winwid = 512;
const int imgwid = 128;
const int imghei = 128;

bool mouse_pressed;
double prev_mousex;
double prev_mousey;

float movement_speed = 0.5; // units per pixel at scale 1
float looking_speed = 0.2; // degrees per pixel
double scaling_speed = 1.1; // coefficient per scroll wheel unit

Stopwatch fps_stopwatch;
const int fps_update_period = 20;
int cur_frame_number;

Camera camera;

std::unique_ptr<RaytracingEngine> raytracer;
std::unique_ptr<Renderer> renderer;

std::unique_ptr<glfw::Window> window;

static void LogGLFWError(int code, const char *message) {
  std::cerr << "glfw error " << code << ": " << message << std::endl;
}

static void KeyCallback(
  GLFWwindow* w, int key, int scancode, int action, int mods
) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    window->SetShouldClose();
}

static void ScrollCallback(GLFWwindow *w, double dx, double dy) {
  if (dy != 0) {
    camera.set_scale(camera.scale() * pow(scaling_speed, dy));
    raytracer->UpdateScale(camera.scale());
  }
}

static void MouseButtonCallback(
  GLFWwindow *w, int button, int action, int mods
) {
  if (button == GLFW_MOUSE_BUTTON_1) {
    if (action == GLFW_PRESS) {
      mouse_pressed = true;
      window->GetCursorPos(&prev_mousex, &prev_mousey);
      window->SetInputMode(GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    } else {
      mouse_pressed = false;
      window->SetCursorPos(prev_mousex, prev_mousey);
    }
  }
}

static void CursorPosCallback(GLFWwindow *w, double x, double y) {
  if (mouse_pressed) {
    double dx = x - prev_mousex;
    double dy = y - prev_mousey;
    if (dx != 0 || dy != 0) {
      window->SetCursorPos(prev_mousex, prev_mousey);
      window->SetInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      camera.set_yaw(camera.yaw() + dx * looking_speed);
      camera.set_pitch(camera.pitch() - dy * looking_speed);
    }
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
    camera.MoveRelative(movement * frame_time * movement_speed);
    raytracer->UpdatePosition(camera.position());
  }
}

static void Render() {
  glClearColor(0,0,0,1);
  glClear(GL_COLOR_BUFFER_BIT);
  //renderer->Render(raytracer->Raytrace(), winwid, winhei);
}

static void UpdateFPS() {
  if (cur_frame_number % fps_update_period == 0) {
    char fps_str[100];
    double fps = 1.0 / fps_stopwatch.Restart() * fps_update_period;
    sprintf(fps_str, "FPS: %lf", fps);
    window->SetTitle(fps_str);
  }
}

int main(int argc, char **argv) {
  try {
    freopen("log.txt", "w", stderr);
    glfwSetErrorCallback(&LogGLFWError);
    glfw::Initializer glfw;
    window.reset(new glfw::Window(winwid, winhei, "upchk"));
    window->MakeCurrent();
    window->GetFramebufferSize(&winwid, &winhei);

    if (glewInit() != GLEW_OK)
      throw GraphicsAPIException("couldn't initialize GLEW");

    camera.set_aspect_ratio(static_cast<float>(winwid) / winhei);
    camera.set_position(fvec3(0, 0, 10));

    raytracer.reset(new RaytracingEngine(
      std::make_shared<cpu_raytracers::Gradient>(),
      imgwid,
      imghei));
    renderer.reset(new Renderer());

    window->SetKeyCallback(&KeyCallback);
    window->SetScrollCallback(&ScrollCallback);
    window->SetMouseButtonCallback(&MouseButtonCallback);
    window->SetCursorPosCallback(&CursorPosCallback);

    Stopwatch frame_stopwatch;  

    while (!window->ShouldClose()) {
      double frame_time = frame_stopwatch.Restart();
      ++cur_frame_number;

      ProcessKeyboardInput(frame_time);
      UpdateFPS();
      Render();

      float ratio = winwid / (float) winhei;
      glViewport(0, 0, winwid, winhei);
      glClear(GL_COLOR_BUFFER_BIT);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
      glBegin(GL_TRIANGLES);
      glColor3f(1.f, 0.f, 0.f);
      glVertex3f(-0.6f, -0.4f, 0.f);
      glColor3f(0.f, 1.f, 0.f);
      glVertex3f(0.6f, -0.4f, 0.f);
      glColor3f(0.f, 0.f, 1.f);
      glVertex3f(0.f, 0.6f, 0.f);
      glEnd();

      window->SwapBuffers();
      glfwPollEvents();
    }
  } catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << std::endl;
    return 2;
  }
}
