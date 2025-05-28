#include "img.hpp"
#include "render/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STBI_FORCE_RGBA 4

/* clang-format off */
std::expected<asset::Asset2D, std::string> asset::loader::Img::tryFromFile(
  const std::filesystem::path& path,
  texture::Manager& texMan
) noexcept { /* clang-format on */
  int width, height, channels;
  unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_FORCE_RGBA);

  if (!data) {
    return std::unexpected{std::format("{}: {}", stbi_failure_reason(), path.string())};
  }

  std::vector<unsigned char> imageData(data, data + (width * height * channels));

  stbi_image_free(data);

  auto textureId = texMan.create(width, height, texture::Format::RGBA, imageData);
  if (!textureId.has_value()) {
    return std::unexpected{std::format("Failed to create texture: {}", textureId.error())};
  }

  return asset::Asset2D(/* clang-format off */
    textureId.value()
  );/* clang-format on */
}

/* clang-format off */
std::expected<asset::Asset2D, std::string> asset::loader::Img::tryFromData(
  const std::vector<std::byte>& data,
  texture::Manager& texMan
) noexcept { /* clang-format on */
  int originalWidth;
  int originalHeight;
  int originalChannels;

  /* clang-format off */
  unsigned char* imageData = stbi_load_from_memory(
    reinterpret_cast<const unsigned char*>(data.data()),
    static_cast<int>(data.size()),
    &originalWidth,
    &originalHeight,
    &originalChannels,
    STBI_FORCE_RGBA
  ); /* clang-format on */

  if (!imageData) {
    return std::unexpected{std::format("Failed to load image: {}", stbi_failure_reason())};
  }

  size_t outputDataSize = originalWidth * originalHeight * 4;
  std::vector<unsigned char> outputData(imageData, imageData + outputDataSize);

  stbi_image_free(imageData);

  auto textureId = texMan.create(originalWidth, originalHeight, texture::Format::RGBA, outputData);
  if (!textureId.has_value()) {
    return std::unexpected{std::format("Failed to create texture: {}", textureId.error())};
  }

  return asset::Asset2D(textureId.value());
}
