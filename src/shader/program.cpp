#include "program.hpp"

#include <glad/glad.h>

namespace shader {
  Program::Program() : programIdx(glCreateProgram()) {
  }

  Program::~Program() {
    glDeleteProgram(programIdx);
  }

  void Program::addShader(std::unique_ptr<Shader> shader) {
    glAttachShader(programIdx, shader->getShaderIdx());
    shaders.push_back(std::move(shader));
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

  void Program::use() const {
    glUseProgram(programIdx);
  }

  bool Program::setUniformMatrix4fv(const std::string& name, const glm::mat4& matrix) {
    GLint location = glGetUniformLocation(programIdx, name.c_str());
    if (location == -1) {
      return false;
    }

    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
    return true;
  }

  uint32_t Program::getProgramIdx() const {
    return programIdx;
  }
}