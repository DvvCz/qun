#pragma once

#include <glad/gl.h>

namespace uniform {
  template <typename T> class Block {
  public:
    Block(const GLint location);
    void set(const T& value) const;

    GLint location;
    GLuint bufferIdx;
  };
}
