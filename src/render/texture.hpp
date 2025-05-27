#pragma once

#include <vector>
#include <string>
#include <expected>

#include <glad/glad.h>

#include "uniform.hpp"
#include "asset/asset.hpp"

namespace texture {

  class Manager {
  public:
    Manager(Uniform<GLuint> sampler2DArrayUniform);
    ~Manager();

    std::expected<GLuint, std::string> addTexture(const asset::Asset2D& texture) noexcept;
    std::optional<GLuint> getTextureByPath(const std::filesystem::path& path) const noexcept;

    void bind();
    void unbind();

  private:
    std::vector<asset::Asset2D> textures;
    GLuint sampler2DArrayIdx;
    GLuint samplerIdx;
    Uniform<GLuint> sampler2DArray;
  };

}
