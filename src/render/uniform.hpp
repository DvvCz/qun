#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

template <typename T> class Uniform;

template <> class Uniform<glm::mat4> {
public:
  Uniform(const GLint location) : location(location) {
  }

  void set(const glm::mat4x4& value) const {
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
  }

private:
  GLint location;
};