#pragma once

#include <vector>
#include <string>
#include <expected>

#include <glad/glad.h>

#include "uniform/single.hpp"
#include "asset/asset.hpp"

namespace texture {

  class Manager {
  public:
    Manager(uniform::Single<GLuint> sampler2DArrayUniform);
    ~Manager();

    std::expected<GLuint, std::string> addTexture(const asset::Asset2D& texture) noexcept;
    std::optional<GLuint> getTextureByPath(const std::filesystem::path& path) const noexcept;

    void bind();
    void unbind();

  private:
    std::vector<asset::Asset2D> textures;
    GLuint sampler2DArrayIdx;
    GLuint samplerIdx;
    uniform::Single<GLuint> sampler2DArray;
  };

}
