#pragma once

#include <vector>
#include <string>
#include <expected>

#include <glad/gl.h>

#include "uniform/single.hpp"

namespace texture {
  // Carefully ensure this is std140
  struct Texture {
    glm::vec2 uvScale = glm::vec2(1.0f, 1.0f);
    glm::vec2 uvOffset = glm::vec2(0.0f, 0.0f);
    GLint index = -1;
    float uvRotation = 0.0f;
    float _padding[2];
  };

  static_assert(sizeof(texture::Texture) % 16 == 0, "Ensure Texture is std140 compliant");

  enum Format {
    R = 0,
    RG = 1,
    RGB = 2,
    RGBA = 3
  };

  using Data = std::vector<unsigned char>;

  class Manager {
  public:
    Manager(uniform::Single<GLint> sampler2DArrayUniform, GLint textureUnit);
    ~Manager();

    /* clang-format off */
    std::expected<texture::Texture, std::string> create(
      int width,
      int height,
      texture::Format format,
      texture::Data data
    ) noexcept; /* clang-format on */

    void bind();
    void unbind();

  private:
    std::vector<bool> textureSlots;
    GLuint sampler2DArrayIdx;
    GLuint samplerIdx;
    uniform::Single<GLint> sampler2DArray;
    GLint textureUnit;
  };

}
