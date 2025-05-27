#pragma once

#include <expected>
#include <string>
#include <filesystem>

#include <rapidobj/rapidobj.hpp>
#include <glm/glm.hpp>

#include "asset/asset.hpp"

#include "render/texture.hpp"

namespace asset::loader {
  class Obj {
  public:
    /* clang-format off */
    [[nodiscard]] static std::expected<asset::Asset3D, std::string> tryFromFile(
      const std::filesystem::path& path,
      texture::Manager& texMan
    ) noexcept; /* clang-format on */
  };
}
