#include "shader-provider.h"
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
  thread_ = std::thread(std::bind(&ShaderProvider::ThreadFunc, this));
  config_subscription_ = config_->Subscribe(
    {{"_dir"}, {"root_dir"}, config_path_},
    std::bind(&ShaderProvider::SetNeedsUpdate, this));
}

ShaderProvider::~ShaderProvider() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    need_update_ = true;
    need_shutdown_ = true;
  }
  cv_.notify_one();
  thread_.join();
}

std::shared_ptr<GL::Shader> ShaderProvider::Get() {
  return std::atomic_load(&shader_);
}

std::shared_ptr<GL::Shader> ShaderProvider::LoadShader(
    Config::Version conf, std::set<std::string> &out_deps) {
  std::string root_dir =
    PathConcat(conf.GetString({"_dir"}), conf.GetString({"root_dir"}));
  std::string vert = ReadFile(PathConcat(root_dir, vert_shader_path_));
  std::string frag_shader_path = conf.GetString(config_path_);
  std::string frag;
  preprocessor_.LoadAndPreprocess(
    frag_shader_path, root_dir, frag, out_deps);
  return std::make_shared<GL::Shader>(
    vert_shader_path_, frag_shader_path, vert, frag);
}

void ShaderProvider::Update() {
  try {
    std::set<std::string> deps;
    auto new_shader = LoadShader(config_->Current(), deps);
    std::atomic_store(&shader_, new_shader);
    if (deps != cur_deps_) {
      cur_deps_ = deps;
      file_watcher_.reset(new FileWatcher(
        std::vector<std::string>(deps.begin(), deps.end()),
        std::bind(&ShaderProvider::SetNeedsUpdate, this)));
    }
  } catch (...) {
    LogCurrentException();
  }
}

void ShaderProvider::ThreadFunc() {
  while (true) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      cv_.wait(lock, [&] { return need_update_; });
      if (need_shutdown_)
        break;
    }

    Update();
  }
}

void ShaderProvider::SetNeedsUpdate() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    need_update_ = true;
  }
  cv_.notify_one();
}

}
