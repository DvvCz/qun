#include "program.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <print>

namespace shader {
  Program::Program() : programIdx(glCreateProgram()) {
  }

  Program::~Program() {
    glDeleteProgram(programIdx);
  }

  void Program::addShader(std::unique_ptr<Shader> shader) {
    glAttachShader(programIdx, shader->getShaderIdx());

    shaders.push_back({/* clang-format off */
      #ifdef SHADER_HOTRELOADING
      .fsLastChanged = static_cast<uint64_t>(std::filesystem::last_write_time(shader->getShaderPath()).time_since_epoch().count()),
      #endif
      .shader = std::move(shader)
    });/* clang-format on */
  }

  void Program::link() {
    glLinkProgram(programIdx);

    int success;
    glGetProgramiv(programIdx, GL_LINK_STATUS, &success);

    if (!success) {
      std::vector<GLchar> errorMessage(512);
      glGetProgramInfoLog(programIdx, 512, NULL, errorMessage.data());

      throw std::runtime_error(std::string("Failed to link program: ") + std::string(errorMessage.begin(), errorMessage.end()));
    }
  }

  void Program::use() const {
    glUseProgram(programIdx);
  }

  uint32_t Program::getProgramIdx() const {
    return programIdx;
  }

#ifdef SHADER_HOTRELOADING
  void Program::checkForHotReload() {
    // Only check every 500ms to avoid spamming the filesystem (although likely optimized by the OS)
    uint64_t curTimeMs = static_cast<uint64_t>(glfwGetTime() * 1000.0);
    if ((lastCheckedForHotReload - curTimeMs) < 500) {
      return;
    }
    lastCheckedForHotReload = curTimeMs;

    for (auto& shader : shaders) {
      auto lastChanged =
          static_cast<uint64_t>(std::filesystem::last_write_time(shader.shader->getShaderPath()).time_since_epoch().count());

      if (lastChanged > shader.fsLastChanged) {
        shader.fsLastChanged = lastChanged;

        auto r = shader.shader->recompile();
        if (!r.has_value()) {
          std::println(stderr, "Failed to recompile shader {}: {}", shader.shader->getShaderPath().string(), r.error());
          continue;
        }
      }
    }

    link();
  }
#endif
}
