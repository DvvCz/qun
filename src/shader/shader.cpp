#include "shader.hpp"

#include <glad/glad.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <utility>
#include <print>
#include <vector>

uint32_t shader::shaderTypeToGLType(const ShaderType type) {
  switch (type) {
  case ShaderType::Vertex:
    return GL_VERTEX_SHADER;
  case ShaderType::Fragment:
    return GL_FRAGMENT_SHADER;
  }

  std::unreachable();
}

shader::Shader::Shader(std::filesystem::path path, const ShaderType type) {
  shaderType = type;
  shaderIdx = glCreateShader(shaderTypeToGLType(type));
  shaderPath = path;

  if (!std::filesystem::exists(path)) {
    throw std::runtime_error("Shader file does not exist: " + path.string());
  }

  auto fileHandle = std::ifstream(path);
  if (!fileHandle.is_open()) {
    throw std::runtime_error("Failed to open shader file: " + path.string());
  }

  std::ostringstream buffer;
  buffer << fileHandle.rdbuf();
  std::string shaderSource = buffer.str();

  auto compileResult = tryCompile(shaderIdx, shaderSource);
  if (!compileResult) {
    throw std::runtime_error("Failed to compile shader " + path.string() + ": " + compileResult.error());
  }
}

shader::Shader::~Shader() {
  glDeleteShader(shaderIdx);
}

uint32_t shader::Shader::getShaderIdx() const {
  return shaderIdx;
}

std::expected<bool, std::string> shader::Shader::tryCompile(uint32_t shaderIdx, std::string source) noexcept {
  auto shaderSourceCStr = source.c_str();

  glShaderSource(shaderIdx, 1, &shaderSourceCStr, NULL);
  glCompileShader(shaderIdx);

  int success;
  glGetShaderiv(shaderIdx, GL_COMPILE_STATUS, &success);

  if (!success) {
    std::vector<GLchar> errorBuf(512);
    glGetShaderInfoLog(shaderIdx, 512, NULL, errorBuf.data());

    auto errorMessage = std::string(errorBuf.begin(), errorBuf.end());
    return std::unexpected(errorMessage);
  }

  return true;
}