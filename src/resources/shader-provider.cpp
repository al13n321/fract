#include "shader-provider.h"
#include "util/string-util.h"

namespace fract {

ShaderProvider::ShaderProvider(
  ConfigPtr config,
  const std::string &vert_shader_path,
  std::initializer_list<std::string> config_path,
  std::initializer_list<std::pair<std::string, std::string>>
    preprocessor_definitions)
  : config_(config)
  , vert_shader_path_(vert_shader_path)
  , config_path_(config_path.begin(), config_path.end())
  , preprocessor_(preprocessor_definitions)
{
  shader_ = LoadShader(config_->Current());
}

std::shared_ptr<GL::Shader> ShaderProvider::Get() {
  return std::atomic_load(&shader_);
}

std::shared_ptr<GL::Shader> ShaderProvider::LoadShader(Config::Version conf) {
  std::string root_dir = conf.GetString({"root_dir"});
  std::string vert = ReadFile(PathConcat(root_dir, vert_shader_path_));
  std::string frag;
  std::set<std::string> deps;
  preprocessor_.LoadAndPreprocess(
    conf.GetString(config_path_), root_dir, frag, deps);
  return std::make_shared<GL::Shader>(vert, frag);
}

}
