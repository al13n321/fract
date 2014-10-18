#pragma once

#include "gl-util/shader.h"
#include "shader-preprocessor.h"
#include "config.h"

namespace fract {

// Preprocesses and compiles a shader. Takes path from config.
// Reloads and recompiles when either the shader source file changes
// or path to it changes in config.
// If the new version doesn't compile keeps the last working one.
// TODO: reloading (don't forget to subscribe to root changes too).
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

  // Throws.
  std::shared_ptr<GL::Shader> LoadShader(Config::Version conf);
};

}
