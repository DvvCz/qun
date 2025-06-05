#include "texture.hpp"

#include <format>
#include <stdexcept>

#define MAX_WIDTH 2048
#define MAX_HEIGHT 2048
#define MAX_TEXTURES 64

texture::Manager::Manager(uniform::Single<GLint> sampler2DUniform, GLint textureUnit)
    : sampler2DArray(sampler2DUniform), textureUnit(textureUnit) {
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

texture::Manager::~Manager() {
  glDeleteTextures(1, &sampler2DArrayIdx);
  glDeleteSamplers(1, &samplerIdx);
}

/* clang-format off */
std::expected<texture::Texture, std::string> texture::Manager::create(
  int width,
  int height,
  texture::Format format,
  texture::Data data
) noexcept { /* clang-format on */
  // todo: allow freeing up slots
  if (textureSlots.size() >= MAX_TEXTURES) {
    return std::unexpected("Maximum number of textures reached");
  }

  if (width > MAX_WIDTH || height > MAX_HEIGHT) {
    return std::unexpected(/* clang-format off */
      std::format(
        "Texture of {}x{} exceeds maximum dimensions of {}x{}",
        width,
        height,
        MAX_WIDTH,
        MAX_HEIGHT
      )
    );/* clang-format on */
  }

  if (data.empty() || width <= 0 || height <= 0) {
    return std::unexpected("Invalid texture data");
  }

  size_t textureId = textureSlots.size();

  GLenum textureFormat;
  switch (format) {
  case texture::Format::RG:
    textureFormat = GL_RG;
    break;
  case texture::Format::RGB:
    textureFormat = GL_RGB;
    break;
  case texture::Format::RGBA:
    textureFormat = GL_RGBA;
    break;
  default:
    return std::unexpected("Invalid number of channels");
  }

  glTextureSubImage3D(/* clang-format off */
    sampler2DArrayIdx,
    0,
    0,
    0,
    textureId,
    width,
    height,
    1,
    textureFormat,
    GL_UNSIGNED_BYTE,
    data.data()
  ); /* clang-format on */

  textureSlots.push_back(true);

  return texture::Texture{/* clang-format off */
    .uvScale = glm::vec2((float)width / (float)MAX_WIDTH, (float)height / (float)MAX_HEIGHT),
    .uvOffset = glm::vec2(0.0f, 0.0f),
    .index = static_cast<GLint>(textureId),
    .uvRotation = 0.0f
  }; /* clang-format on */
}

void texture::Manager::bind() {
  sampler2DArray.set(textureUnit);
  glBindTextureUnit(textureUnit, sampler2DArrayIdx);
  glBindSampler(textureUnit, samplerIdx);
}

void texture::Manager::unbind() {
  glBindTextureUnit(textureUnit, 0);
  glBindSampler(textureUnit, 0);
}
