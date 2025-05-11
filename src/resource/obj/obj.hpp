#pragma once

#include <expected>
#include <string>
#include <vector>
#include <array>
#include <filesystem>

#include "../asset.hpp"

namespace resource {
  class ObjAsset : public ObjectAsset {
  public:
    [[nodiscard]] static std::expected<ObjAsset, std::string> tryFromFile(const std::filesystem::path& path) noexcept;

  private:
    ObjAsset(std::vector<std::array<float, 3>> vertices, std::vector<int> indices);

    std::vector<std::array<float, 3>> vertices;
    std::vector<int> indices;
  };
}