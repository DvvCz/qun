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

    // Override base class methods
    [[nodiscard]] std::vector<std::array<float, 3>> getVertices() const noexcept override;
    [[nodiscard]] std::vector<int> getIndices() const noexcept override;

  private:
    ObjAsset(std::vector<std::array<float, 3>> vertices, std::vector<int> indices);

    std::vector<std::array<float, 3>> vertices;
    std::vector<int> indices;
  };
}