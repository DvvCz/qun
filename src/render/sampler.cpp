#include "sampler.hpp"

#define MAX_WIDTH 1024
#define MAX_HEIGHT 1024
#define MAX_TEXTURES 512

TextureManager::TextureManager(Uniform<GLuint> sampler2DUniform, Uniform<GLint> textureIdxUniform)
    : sampler2DArray(sampler2DUniform), textureIdx(textureIdxUniform) {
  GLint maxLayers;
  glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxLayers);

  if (maxLayers < MAX_TEXTURES) {
    throw std::runtime_error("Maximum texture layers supported is less than MAX_TEXTURES");
  }

  // Create the sampler2DArray and allocate max space
  glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &sampler2DArrayIdx);
  glTextureStorage3D(sampler2DArrayIdx, 1, GL_RGBA8, MAX_WIDTH, MAX_HEIGHT, MAX_TEXTURES);

  glCreateSamplers(1, &samplerIdx);
  glSamplerParameteri(samplerIdx, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glSamplerParameteri(samplerIdx, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glSamplerParameteri(samplerIdx, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glSamplerParameteri(samplerIdx, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

TextureManager::~TextureManager() {
  glDeleteTextures(1, &sampler2DArrayIdx);
  glDeleteSamplers(1, &samplerIdx);
}

std::expected<GLuint, std::string> TextureManager::addTexture(const resource::ImgAsset& texture) noexcept {
  // todo: allow freeing up slots
  if (textures.size() >= MAX_TEXTURES) {
    return std::unexpected("Maximum number of textures reached");
  }

  GLuint textureId = textures.size();
  glTextureSubImage3D(/* clang-format off */
    sampler2DArrayIdx,
    0,
    0,
    0,
    textureId,
    texture.getWidth(),
    texture.getHeight(),
    1,
    GL_RGBA,
    GL_UNSIGNED_BYTE,
    texture.getData().data()
  ); /* clang-format on */

  textures.push_back(texture);
  return textureId;
}

void TextureManager::bindTexture(GLuint textureId) noexcept {
  glBindTextureUnit(0, sampler2DArrayIdx);
  glBindSampler(0, samplerIdx);
  glBindImageTexture(0, sampler2DArrayIdx, 0, GL_FALSE, textureId, GL_READ_ONLY, GL_RGBA8);
  textureIdx.set(textureId);
}