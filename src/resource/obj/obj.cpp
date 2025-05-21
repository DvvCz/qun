#include "obj.hpp"

#include <variant>
#include <optional>
#include <filesystem>
#include <print>

#include <rapidobj/rapidobj.hpp>

std::expected<resource::ObjAsset, std::string> resource::ObjAsset::tryFromFile(const std::filesystem::path& path) noexcept {
  auto obj = rapidobj::ParseFile(path);

  if (obj.error) {
    std::string errMsg = std::format("Error parsing OBJ File: '{}' (at line {})", obj.error.code.message(), obj.error.line_num);
    return std::unexpected{errMsg};
  }

  rapidobj::Triangulate(obj);

  if (obj.error) {
    std::string errMsg =
        std::format("Error triangulating OBJ File: '' (at line {})", obj.error.code.message(), obj.error.line_num);
    return std::unexpected{errMsg};
  }

  std::vector<int> allIndices;

  for (const auto& shape : obj.shapes) {
    for (const auto& index : shape.mesh.indices) {
      allIndices.push_back(static_cast<int>(index.position_index));
    }
  }

  // Convert flat array of positions to vector of std::array<float, 3>
  std::vector<std::array<float, 3>> allVertices;
  allVertices.reserve(obj.attributes.positions.size() / 3);

  for (size_t i = 0; i < obj.attributes.positions.size(); i += 3) {
    if (i + 2 < obj.attributes.positions.size()) {
      std::array<float, 3> vertex = {obj.attributes.positions[i], obj.attributes.positions[i + 1],
                                     obj.attributes.positions[i + 2]};
      allVertices.push_back(vertex);
    }
  }

  return resource::ObjAsset(allVertices, allIndices);
}

resource::ObjAsset::ObjAsset(std::vector<std::array<float, 3>> vertices, std::vector<int> indices)
    : vertices(std::move(vertices)), indices(std::move(indices)) {
}

std::vector<std::array<float, 3>> resource::ObjAsset::getVertices() const noexcept {
  return vertices;
}

std::vector<int> resource::ObjAsset::getIndices() const noexcept {
  return indices;
}