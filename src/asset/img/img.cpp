#include "img.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

std::expected<asset::Asset2D, std::string> asset::Img::tryFromFile(const std::filesystem::path& path) noexcept {
  int width, height, channels;
  unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

  if (!data) {
    return std::unexpected{std::format("{}: {}", stbi_failure_reason(), path.string())};
  }

  std::vector<unsigned char> imageData(data, data + (width * height * channels));

  stbi_image_free(data);

  return asset::Asset2D(/* clang-format off */
    width,
    height,
    channels,
    std::move(imageData),
    path
  );/* clang-format on */
}
