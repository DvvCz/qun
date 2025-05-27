#pragma once

#include <expected>
#include <string>
#include <filesystem>

#include <rapidobj/rapidobj.hpp>
#include <glm/glm.hpp>

#include "asset/asset.hpp"

namespace asset {
  class Obj {
  public:
    [[nodiscard]] static std::expected<asset::Asset3D, std::string> tryFromFile(const std::filesystem::path& path) noexcept;
  };
}
