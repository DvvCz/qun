#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace uniform {
  template <typename T> struct Single;

  template <> struct Single<glm::mat4> {
    void set(const glm::mat4x4& value) const {
      glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    }

    GLint location;
  };

  template <> struct Single<float> {
    void set(const float value) const {
      glUniform1f(location, value);
    }

    GLint location;
  };

  template <> struct Single<GLuint> {
    void set(const GLuint value) const {
      glUniform1ui(location, value);
    }

    GLint location;
  };

  template <> struct Single<GLint> {
    void set(const GLint value) const {
      glUniform1i(location, value);
    }

    GLint location;
  };

  template <> struct Single<glm::vec3> {
    void set(const glm::vec3& value) const {
      glUniform3fv(location, 1, &value[0]);
    }

    GLint location;
  };

  template <> struct Single<glm::vec4> {
    void set(const glm::vec4& value) const {
      glUniform4fv(location, 1, &value[0]);
    }

    GLint location;
  };

  template <> struct Single<glm::vec2> {
    void set(const glm::vec2& value) const {
      glUniform2fv(location, 1, &value[0]);
    }

    GLint location;
  };

}
