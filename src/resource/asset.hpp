#pragma once

#include <vector>
#include <optional>
#include <string>
#include <array>

class Asset {
public:
  Asset();

  [[nodiscard]] std::optional<std::string> getName() const noexcept;
};

class ObjectAsset : public Asset {
public:
  ObjectAsset();

  [[nodiscard]] std::vector<std::array<float, 3>> getVertices() const noexcept;
  [[nodiscard]] std::vector<int> getIndices() const noexcept;

private:
  std::vector<std::array<float, 3>> vertices;
  std::vector<int> indices;
};