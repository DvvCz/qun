#pragma once

#include <expected>
#include <string>
#include <filesystem>

#include <stb_image.h>

#include "asset/asset.hpp"

#include "render/texture.hpp"

namespace asset::loader {
  class Img {
  public:
    /* clang-format off */
    [[nodiscard]] static std::expected<asset::Asset2D, std::string> tryFromFile(
      const std::filesystem::path& path,
      texture::Manager& texMan
    ) noexcept; /* clang-format on */

    /* clang-format off */
    [[nodiscard]] static std::expected<asset::Asset2D, std::string> tryFromData(
      const std::vector<std::byte>& data,
      texture::Manager& texMan
    ) noexcept; /* clang-format on */
  };
}
