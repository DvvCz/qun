#pragma once

#include <expected>
#include <string>
#include <vector>
#include <array>
#include <filesystem>
#include <optional>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include <glm/glm.hpp>

#include "asset/asset.hpp"
#include "render/vertex.hpp"

namespace asset {
  class Gltf : public Asset {
  public:
    [[nodiscard]] std::optional<std::string> getName() const noexcept {
      return "GLTF Asset";
    }

    [[nodiscard]] static std::expected<Gltf, std::string> tryFromFile(const std::filesystem::path& path) noexcept;

    [[nodiscard]] std::filesystem::path getPath() const noexcept {
      return path;
    }

  private:
    std::filesystem::path path;
  };
}
