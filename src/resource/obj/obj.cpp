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

  std::vector<Vertex> vertices;
  std::vector<ObjShape> shapes;

  for (const auto& shape : obj.shapes) {
    std::vector<int> shapeIndices;

    for (const auto& index : shape.mesh.indices) {
      auto vertexIndex = vertices.size();

      vertices.push_back(Vertex{
          .pos = glm::vec3(obj.attributes.positions[index.position_index]),
          .normal = glm::vec3(obj.attributes.normals[index.normal_index]),
          .uv = glm::vec2(obj.attributes.texcoords[index.texcoord_index]),
      });

      shapeIndices.push_back(vertexIndex);
    }

    shapes.push_back(ObjShape{
        .name = shape.name,
        .indices = std::move(shapeIndices),
    });
  }

  return ObjAsset{std::move(vertices), std::move(shapes)};
}