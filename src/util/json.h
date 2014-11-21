#pragma once

#include <json/json.h>
#include "vec.h"
#include "quat.h"

namespace fract {

class JsonUtil {
 public:
  static double doubleValue(const Json::Value &value);
  static double doubleValue(const Json::Value &value, double default_);

  static int intValue(const Json::Value &value);
  static int intValue(const Json::Value &value, int default_);
  
  static dvec3 vec3Value(const Json::Value &value);
  static dvec3 vec3Value(const Json::Value &value, dvec3 default_);

  // Accepted format:
  //  - [1024, 768]
  //  - {"w": 1024, "h": 768}
  //  - {"width": 1024, "height": 768}
  //  - {"x": 1024, "y": 768}
  static ivec2 sizeValue(const Json::Value &value);
  static ivec2 sizeValue(const Json::Value &value, ivec2 default_);

  // [a, b, c, d]
  static fquat quatValue(const Json::Value &value);
  static fquat quatValue(const Json::Value &value, fquat default_);
};

}
