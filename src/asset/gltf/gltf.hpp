#pragma once

#include <expected>
#include <string>
#include <filesystem>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include <glm/glm.hpp>

#include "asset/asset.hpp"

namespace asset {
  class Gltf {
  public:
    [[nodiscard]] static std::expected<asset::Asset3D, std::string> tryFromFile(const std::filesystem::path& path) noexcept;
  };
}
