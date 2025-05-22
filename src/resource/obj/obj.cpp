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

  std::vector<rapidobj::Material> materials;
  for (const auto& material : obj.materials) {
    materials.push_back(material);
  }

  std::vector<Vertex> vertices;
  std::vector<ObjShape> shapes;

  for (const auto& shape : obj.shapes) {
    std::vector<int> shapeIndices;

    for (const auto& index : shape.mesh.indices) {
      auto vertexIndex = vertices.size();

      /* clang-format off */
      glm::vec3 position = glm::vec3(
        obj.attributes.positions[index.position_index * 3],
        obj.attributes.positions[index.position_index * 3 + 1],
        obj.attributes.positions[index.position_index * 3 + 2]
      );/* clang-format on */

      // Default values for normal and UV in case they're not provided
      glm::vec3 normal = position;
      glm::vec2 uv = glm::vec2(0.0f, 0.0f);

      // Check if normal index is valid and normals array is not empty
      if (index.normal_index >= 0) {
        normal = glm::vec3(/* clang-format off */
          obj.attributes.normals[index.normal_index * 3],
          obj.attributes.normals[index.normal_index * 3 + 1],
          obj.attributes.normals[index.normal_index * 3 + 2]
        );/* clang-format on */
      }

      // Check if texcoord index is valid and texcoords array is not empty
      if (index.texcoord_index >= 0) {
        uv = glm::vec2(/* clang-format off */
          obj.attributes.texcoords[index.texcoord_index * 2],
          obj.attributes.texcoords[index.texcoord_index * 2 + 1]
        ); /* clang-format on */
      }

      vertices.push_back(Vertex{
          .pos = position,
          .normal = normal,
          .uv = uv,
      });

      shapeIndices.push_back(vertexIndex);
    }

    std::vector<int> materialIds;
    for (const auto& materialId : shape.mesh.material_ids) {
      materialIds.push_back(materialId);
    }

    shapes.push_back(ObjShape{/* clang-format off */
        .name = shape.name,
        .indices = std::move(shapeIndices),
        .materialIds = materialIds
    });/* clang-format on */
  }

  return ObjAsset{std::move(vertices), std::move(shapes), std::move(materials)};
}