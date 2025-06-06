#include "shader.hpp"

#include <glad/gl.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <utility>
#include <print>
#include <vector>
#include <thread>

uint32_t shader::shaderTypeToGLType(const shader::Type type) {
  switch (type) {
  case shader::Type::Vertex:
    return GL_VERTEX_SHADER;
  case shader::Type::Fragment:
    return GL_FRAGMENT_SHADER;
  }

  std::unreachable();
}

void watchForHotreloading(const std::filesystem::path& path, shader::Shader& shader, std::atomic<bool>& isWatching) {
  auto lastWriteTime = std::filesystem::last_write_time(path);

  while (true) {
    auto currentWriteTime = std::filesystem::last_write_time(path);
    if (currentWriteTime != lastWriteTime) {
      lastWriteTime = currentWriteTime;

      auto result = shader.recompile();
      if (!result.has_value()) {
        std::println(stderr, "Failed to recompile shader {}: {}", path.string(), result.error());
        continue;
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
}

shader::Shader::Shader(std::filesystem::path path, const shader::Type type) {
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

std::expected<bool, std::string> shader::Shader::recompile() {
  auto fileHandle = std::ifstream(shaderPath);
  if (!fileHandle.is_open()) {
    throw std::runtime_error("Failed to open shader file for recompilation: " + shaderPath.string());
  }

  std::ostringstream buffer;
  buffer << fileHandle.rdbuf();
  std::string shaderSource = buffer.str();

  auto compileResult = tryCompile(shaderIdx, shaderSource);
  if (!compileResult.has_value()) {
    return compileResult;
  }

  return true;
}
