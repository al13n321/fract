#pragma once

#include <thread>
#include "gl-util/shader.h"
#include "shader-preprocessor.h"
#include "config.h"

namespace fract {

// Preprocesses and compiles a shader. Takes path from config.
// Reloads and recompiles when either the shader source file changes
// or path to it changes in config.
// If the new version doesn't compile keeps the last working one.
// Does the compilation synchronously in Get(). Doing it in background would
// require multiple GL contexts and would usually be single-threaded in
// the driver anyway.
class ShaderProvider {
 public:
  ShaderProvider(
    ConfigPtr config,
    const std::string &vert_shader_path,
    std::initializer_list<std::string> config_path,
    std::initializer_list<std::pair<std::string, std::string>>
      preprocessor_definitions);

  std::shared_ptr<GL::Shader> Get();
 private:
  ConfigPtr config_;
  std::string vert_shader_path_;
  std::vector<std::string> config_path_;
  ShaderPreprocessor preprocessor_;

  std::shared_ptr<GL::Shader> shader_;

  Config::SubscriptionPtr config_subscription_;
  std::unique_ptr<FileWatcher> file_watcher_;
  std::set<std::string> cur_deps_;

  std::atomic<bool> need_update_{};

  void Update();
};

}
