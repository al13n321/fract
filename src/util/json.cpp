#include "json.h"
#include <cassert>
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

int JsonUtil::intValue(const Json::Value &value) {
  if (value.isIntegral())
    return value.asInt();
  else if (value.isString())
    return atoi(value.asString().c_str());
  else
    throw ConfigValueFormatException(
      "expected int value, got " + value.toStyledString());
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

static Json::Value coalesce(
    std::initializer_list<Json::Value> values) {
  assert(values.size() > 0);
  Json::Value res;
  for (auto it: values) {
    if (it.isNull())
      continue;
    if (!res.isNull())
      throw ConfigValueFormatException("more than one alternative provided");
    res = it;
  }
  return res;
}

ivec2 JsonUtil::sizeValue(const Json::Value &value) {
  ivec2 res;
  if (value.isArray()) {
    res = ivec2(
      intValue(value[0]),
      intValue(value[1]));
  } else if (value.isObject()) {
    res = ivec2(
      intValue(coalesce({value["x"], value["w"], value["width"]})),
      intValue(coalesce({value["y"], value["h"], value["height"]})));
  } else {
    throw ConfigValueFormatException(
      "expected vec3 value, got " + value.toStyledString());
  }
  if (res.x < 0 || res.y < 0)
    throw ConfigValueFormatException(
      "negative size: " + value.toStyledString());
  return res;
}

fquat JsonUtil::quatValue(const Json::Value &value) {
  if (value.isArray()) {
    return fquat(
      (float)doubleValue(value[0]),
      (float)doubleValue(value[1]),
      (float)doubleValue(value[2]),
      (float)doubleValue(value[3]));
  } else {
    throw ConfigValueFormatException(
      "expected quaternion value, got " + value.toStyledString());
  }
}


double JsonUtil::doubleValue(const Json::Value &value, double default_) {
  return value.isNull() ? default_ : doubleValue(value);
}
int JsonUtil::intValue(const Json::Value &value, int default_) {
  return value.isNull() ? default_ : intValue(value);
}

dvec3 JsonUtil::vec3Value(const Json::Value &value, dvec3 default_) {
  return value.isNull() ? default_ : vec3Value(value);
}

ivec2 JsonUtil::sizeValue(const Json::Value &value, ivec2 default_) {
  return value.isNull() ? default_ : sizeValue(value);
}

fquat JsonUtil::quatValue(const Json::Value &value, fquat default_) {
  return value.isNull() ? default_ : quatValue(value);
}

}
