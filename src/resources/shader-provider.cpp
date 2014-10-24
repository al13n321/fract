#include "shader-provider.h"
#include <iostream>
#include "util/exceptions.h"
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
  config_subscription_ = config_->Subscribe(
    {{"_dir"}, {"root_dir"}, config_path_},
    [&](Config::Version){ need_update_.store(true); });
}

std::shared_ptr<GL::Shader> ShaderProvider::Get() {
  if (need_update_.exchange(false))
    Update();

  return shader_;
}

void ShaderProvider::Update() {
  std::set<std::string> deps;

  try {
    auto conf = config_->Current();
    std::string root_dir =
      PathConcat(conf.GetString({"_dir"}), conf.GetString({"root_dir"}));
    std::string frag_shader_path = conf.GetString(config_path_);
    std::cerr << "loading " << frag_shader_path << std::endl;
    std::string vert = ReadFile(PathConcat(root_dir, vert_shader_path_));
    std::string frag;
    preprocessor_.LoadAndPreprocess(
      frag_shader_path, root_dir, frag, deps);

    shader_ = std::make_shared<GL::Shader>(
      vert_shader_path_, frag_shader_path, vert, frag);
  } catch (...) {
    LogCurrentException();
  }

  // Need to start watching for updates even if something failed.
  if (deps != cur_deps_) {
    cur_deps_ = deps;
    file_watcher_.reset(new FileWatcher(
      std::vector<std::string>(deps.begin(), deps.end()),
      [&]{ need_update_.store(true); }));
  }
}

}
