#pragma once

#include <expected>
#include <string>
#include <filesystem>

#include <stb/stb_image.h>
#include "../asset.hpp"

namespace asset::loader {
  class Img {
  public:
    [[nodiscard]] static std::expected<asset::Asset2D, std::string> tryFromFile(const std::filesystem::path& path) noexcept;
  };
}
