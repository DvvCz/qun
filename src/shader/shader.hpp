#pragma once

#include <filesystem>
#include <expected>

namespace shader {
  enum class ShaderType {
    Vertex,
    Fragment
  };

  uint32_t shaderTypeToGLType(const ShaderType type);

  class Shader {
  public:
    Shader(std::filesystem::path path, const ShaderType type);

  private:
    std::filesystem::path shaderPath;
    ShaderType shaderType;
    uint32_t shaderIdx;

    [[nodiscard]] static std::expected<bool, std::string> tryCompile(uint32_t shaderIdx, std::string content) noexcept;
  };

  class ShaderGroup {}
}