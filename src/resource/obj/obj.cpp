#include "obj.hpp"

#include <variant>
#include <optional>
#include <filesystem>
#include <print>
#include <map>

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

  std::vector<ObjMaterial> materials;
  for (const auto& material : obj.materials) {

    std::optional<std::filesystem::path> diffuseTexture = std::nullopt;
    if (!material.diffuse_texname.empty()) {
      auto unresolvedPath = std::filesystem::path(material.diffuse_texname);
      diffuseTexture = path.parent_path() / unresolvedPath;
    }

    ObjMaterial mat = {/* clang-format off */
        .name = material.name,

        .ambient = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]),
        .diffuse = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]),
        .specular = glm::vec3(material.specular[0], material.specular[1], material.specular[2]),
        .shininess = material.shininess,
        .dissolve = material.dissolve,

        .diffuseTexture = diffuseTexture
    };/* clang-format on */

    materials.push_back(mat);
  }

  std::vector<Vertex3D> vertices;
  std::vector<ObjShape> shapes;

  for (const auto& shape : obj.shapes) {
    // Create a map of material ID to vector of indices
    std::map<int, std::vector<int>> materialGroups;

    for (size_t i = 0; i < shape.mesh.indices.size(); i++) {
      const auto& index = shape.mesh.indices[i];
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

      vertices.push_back(Vertex3D{
          .pos = position,
          .normal = normal,
          .uv = uv,
      });

      // Get material ID for the current face (each triangle/3 vertices)
      int faceIndex = i / 3;
      int materialId = 0; // Default material ID
      if (faceIndex < static_cast<int>(shape.mesh.material_ids.size())) {
        materialId = shape.mesh.material_ids[faceIndex];
      }

      // Add vertex index to the appropriate material group
      materialGroups[materialId].push_back(vertexIndex);
    }

    // Convert the map to the vector of ObjMaterialGroup
    std::vector<ObjMaterialGroup> groups;
    for (const auto& [materialId, indices] : materialGroups) {
      groups.push_back(ObjMaterialGroup{.materialId = materialId, .indices = indices});
    }

    shapes.push_back(ObjShape{/* clang-format off */
        .name = shape.name,
        .groups = std::move(groups)
    });/* clang-format on */
  }

  return ObjAsset{/* clang-format off */
    std::move(vertices),
    std::move(shapes),
    std::move(materials),
    std::move(path)
  };/* clang-format on */
}
