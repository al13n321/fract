#pragma once
#include <string>
#include <vector>
#include <sstream>

namespace fract {

std::string ReadFile(const std::string &file);
void WriteFile(const std::string &file, const std::string &contents);

// doesn't remove empty tokens
std::vector<std::string> Tokenize(const std::string &s, const std::string &delims);
  
std::string RemoveFileNameFromPath(const std::string &path_with_file_name);
std::string PathConcat(const std::string &path1, const std::string &path2);
std::string PathConcat(std::initializer_list<std::string> paths);

bool StartsWith(const std::string &string, const std::string &prefix);

bool IsWhitespace(char c);
std::string Strip(std::string s);

template <typename T>
std::string ToString(const T &x) {
  std::stringstream ss;
  ss << x;
  return ss.str();
}

template <typename T>
std::string ToString(const std::vector<T> &vec) {
  std::stringstream ss;
  bool first = true;
  for (const T &x: vec) {
    if (first)
      first = false;
    else
      ss << ',';
    ss << ToString(x);
  }
  return ss.str();
}

}
