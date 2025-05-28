#pragma once

#include <vector>
#include <string>
#include <expected>

#include <glad/glad.h>

#include "uniform/single.hpp"
#include "asset/asset.hpp"

namespace texture {
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
    std::expected<size_t, std::string> create(
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
