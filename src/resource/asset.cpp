#include "asset.hpp"
#include <print>

Asset::Asset::Asset() {
}

ObjectAsset::ObjectAsset::ObjectAsset() {
}

std::vector<int> ObjectAsset::getIndices() const noexcept {
  return indices;
}

std::vector<std::array<float, 3>> ObjectAsset::getVertices() const noexcept {
  return vertices;
}

std::optional<std::string> Asset::getName() const noexcept {
  return std::nullopt;
}