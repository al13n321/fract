#include "raytracer/raytracing-engine.h"
#include "raytracer/renderer.h"
#include "cpu-raytracers/test-raytracers.h"
#include "util/camera.h"
#include "util/stopwatch.h"

using namespace fract;

const int winhei = 512;
const int winwid = 512;
const int imgwid = 128;
const int imghei = 128;

int main_window;

int prev_mousex;
int prev_mousey;
bool key_pressed[256];
Stopwatch frame_stopwatch;
    
float movement_speed = 0.5; // units per pixel at scale 1
float looking_speed = 0.2; // degrees per pixel
double scaling_speed = 1.1; // coefficient per scroll wheel unit

Stopwatch fps_stopwatch;
const int fps_update_period = 20;
int cur_frame_number;

Camera camera;

std::unique_ptr<RaytracingEngine> raytracer;
std::unique_ptr<Renderer> renderer;

static void DisplayFunc() {
  glClearColor(0,0,0,1);
  glClear(GL_COLOR_BUFFER_BIT);
  renderer->Render(raytracer->Raytrace());
  glutSwapBuffers();
}

static void IdleFunc() {
  fvec3 movement(0, 0, 0);
  if (key_pressed['a'])
    movement.x -= 1;
  if (key_pressed['d'])
    movement.x += 1;
  if (key_pressed['w'])
    movement.z -= 1;
  if (key_pressed['s'])
    movement.z += 1;
  double frame_time = frame_stopwatch.Restart();

  camera.MoveRelative(movement * frame_time * movement_speed);
  renderer->UpdatePositionAndScale(camera.position(), camera.scale());

  if (cur_frame_number % fps_update_period == 0) {
    char fps_str[100];
    double fps = 1.0 / fps_stopwatch.Restart() * fps_update_period;
    sprintf(fps_str, "%lf", fps);
    string title = string("FPS: ") + fps_str;

    glutSetWindowTitle(title.c_str());
  }

  ++cur_frame_number;
  glutPostRedisplay();
}

static void KeyboardFunc(unsigned char key, int x, int y) {
  if (key == 27) {
    context->WaitForAll();
    exit(0);
  }
  key_pressed[key] = true;
}

static void KeyboardUpFunc(unsigned char key, int x, int y) {
  key_pressed[key] = false;
}

static void MouseFunc(int button, int state, int x, int y) {
  if (button == 3 || button == 4) {
    if (state == GLUT_UP)
      return;
    if (button == 3)
      camera.set_scale(camera.scale() * scaling_speed);
    else
      camera.set_scale(camera.scale() / scaling_speed);
    raytracer->UpdatePositionAndScale(camera.position(), camera.scale());
    return;
  }

  prev_mousex = x;
  prev_mousey = y;
}

static void MouseMotionFunc(int x, int y) {
  camera.set_yaw(camera.yaw() + (x - prev_mousex) * looking_speed);
  camera.set_pitch(camera.pitch() - (y - prev_mousey) * looking_speed);

  prev_mousex = x;
  prev_mousey = y;
}

int main(int argc, char **argv) {
  try {
    freopen("log.txt", "w", stdout);
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(winwid, winhei);
    main_window = glutCreateWindow("upchk");
    glutDisplayFunc(DisplayFunc);
    glutIdleFunc(IdleFunc);
    glutMouseFunc(MouseFunc);
    glutMotionFunc(MouseMotionFunc);
    glutKeyboardFunc(KeyboardFunc);
    glutKeyboardUpFunc(KeyboardUpFunc);
    
    if (glewInit() != GLEW_OK)
      throw VideocardAPICapabilityException("couldn't initialize GLEW");

    camera.set_aspect_ratio(static_cast<float>(winwid) / winhei);
    camera.set_position(0, 0, 10);

    raytracer.reset(new RaytracingEngine(
      std::make_shared<cpu_raytracers::Gradient>(),
      imgwid,
      imghei));
    renderer.reset(new Renderer());

    glutMainLoop();
  } catch (std::exception &e) {
    std::cout << "exception: " << e.what() << std::endl;
    return 2;
  }
}
