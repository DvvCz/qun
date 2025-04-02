#pragma once

#include <filesystem>
#include <expected>
#include <string>
#include <optional>

namespace shader {
  enum class ShaderType {
    Vertex,
    Fragment
  };

  uint32_t shaderTypeToGLType(const ShaderType type);

  class Shader {
  public:
    Shader(std::filesystem::path path, const ShaderType type);
    ~Shader();
    [[nodiscard]] uint32_t getShaderIdx() const;

  private:
    std::filesystem::path shaderPath;
    ShaderType shaderType;
    uint32_t shaderIdx;

    [[nodiscard]] static std::expected<bool, std::string> tryCompile(uint32_t shaderIdx, std::string content) noexcept;
  };
}