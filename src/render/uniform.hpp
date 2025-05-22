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

  GLint location;
};

template <> class Uniform<GLuint> {
public:
  Uniform(const GLint location) : location(location) {
  }

  void set(const GLuint value) const {
    glUniform1ui(location, value);
  }

  GLint location;
};

template <> class Uniform<GLint> {
public:
  Uniform(const GLint location) : location(location) {
  }

  void set(const GLint value) const {
    glUniform1i(location, value);
  }

  GLint location;
};