#include "img.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

std::expected<resource::ImgAsset, std::string> resource::ImgAsset::tryFromFile(const std::filesystem::path& path) noexcept {
  int width, height, channels;
  unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);

  if (!data) {
    return std::unexpected{stbi_failure_reason()};
  }

  std::vector<unsigned char> imageData(data, data + (width * height * channels));

  stbi_image_free(data);

  return ImgAsset(/* clang-format off */
    width,
    height,
    channels,
    std::move(imageData)
  );/* clang-format on */
}