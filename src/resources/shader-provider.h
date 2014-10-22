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
class ShaderProvider {
 public:
  ShaderProvider(
    ConfigPtr config,
    const std::string &vert_shader_path,
    std::initializer_list<std::string> config_path,
    std::initializer_list<std::pair<std::string, std::string>>
      preprocessor_definitions);
  ~ShaderProvider();

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

  std::thread thread_;
  bool need_update_{};
  bool need_shutdown_{};
  std::mutex mutex_; // only for need_* and cv_
  std::condition_variable cv_;

  // Throws.
  std::shared_ptr<GL::Shader> LoadShader(
    Config::Version conf, std::set<std::string> &out_deps);
  void Update();

  void ThreadFunc();
  void SetNeedsUpdate();
};

}
