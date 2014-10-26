#include "json.h"
#include "exceptions.h"

namespace fract {

double JsonUtil::doubleValue(const Json::Value &value) {
  // isDouble includes integers.
  if (value.isDouble() || value.isIntegral())
    return value.asDouble();
  else if (value.isString())
    return atof(value.asString().c_str());
  else
    throw ConfigValueFormatException(
      "expected double value, got " + value.toStyledString());
}

dvec3 JsonUtil::vec3Value(const Json::Value &value) {
  if (value.isArray()) {
    return dvec3(
      doubleValue(value[0]),
      doubleValue(value[1]),
      doubleValue(value[2]));
  } else if (value.isObject()) {
    return dvec3(
      doubleValue(value["x"]),
      doubleValue(value["y"]),
      doubleValue(value["z"]));
  } else {
    throw ConfigValueFormatException(
      "expected vec3 value, got " + value.toStyledString());
  }
}

}
