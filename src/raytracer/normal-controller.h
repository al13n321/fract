#pragma once

#include "controller.h"
#include "util/config.h"

namespace fract {

// Controller that renders to a window.
class NormalController : public Controller {
 public:
  NormalController(ConfigPtr config);



 private:
  ConfigPtr config_;
  Raytracer tracer_;
  RaytracedView view_;
};

}
