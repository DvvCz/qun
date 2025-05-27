#include "img.hpp"
#include "render/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

static texture::Format getTextureFormat(int channels) {
  switch (channels) {
  case 1:
    return texture::Format::R;
  case 2:
    return texture::Format::RG;
  case 3:
    return texture::Format::RGB;
  case 4:
    return texture::Format::RGBA;
  default:
    return texture::Format::RGBA; // Fallback to RGBA
  }
}

/* clang-format off */
std::expected<asset::Asset2D, std::string> asset::loader::Img::tryFromFile(
  const std::filesystem::path& path,
  texture::Manager& texMan
) noexcept { /* clang-format on */
  int width, height, channels;
  unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

  if (!data) {
    return std::unexpected{std::format("{}: {}", stbi_failure_reason(), path.string())};
  }

  std::vector<unsigned char> imageData(data, data + (width * height * channels));

  stbi_image_free(data);

  auto textureId = texMan.create(width, height, getTextureFormat(channels), imageData);
  if (!textureId.has_value()) {
    return std::unexpected{std::format("Failed to create texture: {}", textureId.error())};
  }

  return asset::Asset2D(/* clang-format off */
    textureId.value()
  );/* clang-format on */
}

/* clang-format off */
std::expected<asset::Asset2D, std::string> asset::loader::Img::tryFromData(
  const std::vector<unsigned char>& data,
  int width,
  int height,
  texture::Format format,
  texture::Manager& texMan
) noexcept { /* clang-format on */
  auto textureId = texMan.create(width, height, format, data);
  if (!textureId.has_value()) {
    return std::unexpected{textureId.error()};
  }

  return asset::Asset2D(/* clang-format off */
    textureId.value()
  );/* clang-format on */
}
