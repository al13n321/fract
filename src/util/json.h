#pragma once

#include <json/json.h>
#include "vec.h"

namespace fract {

class JsonUtil {
 public:
  static double doubleValue(const Json::Value &value);
  static dvec3 vec3Value(const Json::Value &value);
};

}
