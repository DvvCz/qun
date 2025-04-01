#include "shader.hpp"

#include <glad/glad.h>
#include <cstdint>
#include <fstream>
#include <iostream>

uint32_t shader::shaderTypeToGLType(const ShaderType type) {
  switch (type) {
  case ShaderType::Vertex:
    return GL_VERTEX_SHADER;
  case ShaderType::Fragment:
    return GL_FRAGMENT_SHADER;
  }
}

shader::Shader::Shader(std::filesystem::path path, const ShaderType type) {
  shaderType = type;
  shaderIdx = glCreateShader(shaderTypeToGLType(type));
  shaderPath = path;

  auto fileHandle = std::ifstream(path);
  std::ostringstream buffer;
  buffer << fileHandle.rdbuf();
  std::string shaderSource = buffer.str();

  auto compileResult = tryCompile(shaderIdx, shaderSource);
  if (!compileResult) {
    throw std::runtime_error("Failed to compile shader " + path.string() + ": " + compileResult.error());
  }
}

std::expected<bool, std::string> shader::Shader::tryCompile(uint32_t shaderIdx, std::string source) noexcept {
  auto shaderSourceCStr = source.c_str();

  glShaderSource(shaderIdx, 1, &shaderSourceCStr, NULL);
  glCompileShader(shaderIdx);

  int success;
  glGetShaderiv(shaderIdx, GL_COMPILE_STATUS, &success);

  if (!success) {
    std::string errorMessage;
    errorMessage.reserve(512);

    glGetShaderInfoLog(shaderIdx, 512, NULL, errorMessage.data());

    return std::unexpected(errorMessage);
  }

  return true;
}