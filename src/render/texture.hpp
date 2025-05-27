#pragma once

#include <vector>
#include <string>

#include <glad/glad.h>
#include "uniform.hpp"
#include "../resource/img/img.hpp"

class TextureManager {
public:
  TextureManager(Uniform<GLuint> sampler2DArrayUniform);
  ~TextureManager();

  std::expected<GLuint, std::string> addTexture(const resource::ImgAsset& texture) noexcept;
  std::optional<GLuint> getTextureByPath(const std::filesystem::path& path) const noexcept;

  void bind();
  void unbind();

private:
  std::vector<resource::ImgAsset> textures;
  GLuint sampler2DArrayIdx;
  GLuint samplerIdx;
  Uniform<GLuint> sampler2DArray;
};
