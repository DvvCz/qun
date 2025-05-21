#pragma once

#include <expected>
#include <string>
#include <vector>
#include <array>
#include <filesystem>
#include <optional>

#include "../asset.hpp"

namespace resource {
  class ObjAsset : public Asset {
  public:
    [[nodiscard]] static std::expected<ObjAsset, std::string> tryFromFile(const std::filesystem::path& path) noexcept;

    [[nodiscard]] std::vector<std::array<float, 3>> getVertices() const noexcept;
    [[nodiscard]] std::vector<int> getIndices() const noexcept;
    [[nodiscard]] std::optional<std::string> getName() const noexcept override {
      return std::nullopt;
    };

  private:
    ObjAsset(std::vector<std::array<float, 3>> vertices, std::vector<int> indices);

    std::vector<std::array<float, 3>> vertices;
    std::vector<int> indices;
  };
}