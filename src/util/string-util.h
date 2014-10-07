#pragma once
#include <string>
#include <vector>

namespace fract {

std::string ReadFile(const std::string &file);

// doesn't remove empty tokens
std::vector<std::string> Tokenize(std::string &s, const std::string &delims);
  
std::string RemoveFileNameFromPath(std::string path_with_file_name);
std::string CombinePaths(std::string path1, std::string path2);

}
