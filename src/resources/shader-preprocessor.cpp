#include "shader-preprocessor.h"
#include "util/string-util.h"
#include "util/exceptions.h"

namespace fract {

ShaderPreprocessor::ShaderPreprocessor(
  std::initializer_list<std::pair<std::string, std::string>>
    definitions): definitions_(definitions.begin(), definitions.end()) {}

void ShaderPreprocessor::LoadAndPreprocess(
  const std::string &path,
  const std::string &root_path,
  std::string &out_code,
  std::set<std::string> &out_dependencies
) {
  out_dependencies.clear();
  out_code = LoadAndPreprocessRecursive(path, root_path, out_dependencies);
}

std::string ShaderPreprocessor::LoadAndPreprocessRecursive(
  const std::string &path,
  const std::string &root_path,
  std::set<std::string> &included
) {
  if (included.count(path)) {
    return "// " + path + " is already included\n";
  }
  included.insert(path);
  std::vector<std::string> lines =
    Tokenize(ReadFile(PathConcat(root_path, path)), "\n\r");
  std::string res;
  for (const std::string &line: lines) {
    if (StartsWith(line, "#include")) {
      std::string inc = Strip(line.substr(strlen("#include")));
      if (inc.empty())
        throw PreprocessorSyntaxException("empty #include in " + path);
      if (inc[0] == '[') {
        if (inc[inc.size() - 1] != ']')
          throw PreprocessorSyntaxException("unmatched [ in " + path);
        inc = Strip(inc.substr(1, inc.size() - 2));
        if (!definitions_.count(inc))
          throw PreprocessorUndefinedTokenException(
            "[" + inc + "] is undefined in " + path);
        inc = definitions_[inc];
      } else if (inc[0] == '<') {
        if (inc[inc.size() - 1] != '>')
          throw PreprocessorSyntaxException("unmatched < in " + path);
        inc = Strip(inc.substr(1, inc.size() - 2));
      } else
        throw PreprocessorSyntaxException("invalid #include in " + path);

      res += LoadAndPreprocessRecursive(inc, root_path, included);
      res += "\n";
    } else {
      res += line;
    }
  }

  return res;
}

}
