#pragma once

#include <filesystem>
#include <expected>
#include <string>

namespace shader {
  enum class Type {
    Vertex,
    Fragment
  };

  uint32_t shaderTypeToGLType(const shader::Type type);

  class Shader {
  public:
    Shader(std::filesystem::path path, const shader::Type type);
    ~Shader();

    [[nodiscard]] std::expected<bool, std::string> recompile();
    [[nodiscard]] uint32_t getShaderIdx() const;

    [[nodiscard]] const std::filesystem::path& getShaderPath() const {
      return shaderPath;
    }

    [[nodiscard]] shader::Type getShaderType() const {
      return shaderType;
    }

  private:
    std::filesystem::path shaderPath;
    shader::Type shaderType;
    uint32_t shaderIdx;

    [[nodiscard]] static std::expected<bool, std::string> tryCompile(uint32_t shaderIdx, std::string content) noexcept;
  };
}
