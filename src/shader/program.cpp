#include "program.hpp"

#include <glad/glad.h>

namespace shader {
  Program::Program() : programIdx(glCreateProgram()) {
  }

  Program::~Program() {
    glDeleteProgram(programIdx);
  }

  void Program::addShader(const Shader& shader) {
    glAttachShader(programIdx, shader.getShaderIdx());
    shaders.push_back(shader);
  }

  void Program::link() {
    glLinkProgram(programIdx);

    int success;
    glGetProgramiv(programIdx, GL_LINK_STATUS, &success);

    if (!success) {
      std::vector<GLchar> errorMessage(512);
      glGetProgramInfoLog(programIdx, 512, NULL, errorMessage.data());

      throw std::runtime_error(std::string("Failed to link program: ") +
                               std::string(errorMessage.begin(), errorMessage.end()));
    }
  }

  uint32_t Program::getProgramIdx() const {
    return programIdx;
  }
}