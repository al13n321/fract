#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

namespace fract {

class ShaderPreprocessor {
 public:
  ShaderPreprocessor(
    std::initializer_list<std::pair<std::string, std::string>>
      definitions);

  // Throws exception on syntax or I/O error.
  // In this case out_dependencies are still populated with dependencies
  // detected before the error.
  void LoadAndPreprocess(
    const std::string &path,
    const std::string &root_path,
    std::string &out_code,
    std::set<std::string> &out_dependencies); // Included files.
 private:
  std::map<std::string, std::string> definitions_;

  std::string LoadAndPreprocessRecursive(
    const std::string &path,
    const std::string &root_path,
    std::set<std::string> &included);
};

}
