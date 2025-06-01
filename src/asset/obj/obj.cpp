#include "obj.hpp"

#include <optional>
#include <filesystem>
#include <map>

#include <rapidobj/rapidobj.hpp>

#include "asset/asset.hpp"
#include "asset/img/img.hpp"

#include "util/error.hpp"

static glm::vec3 convertFromObj(float x, float y, float z) noexcept {
  return glm::vec3(x, -z, y);
};

/* clang-format off */
std::expected<asset::Asset3D, std::string> asset::loader::Obj::tryFromFile(
  const std::filesystem::path& path,
  texture::Manager& texMan
) noexcept { /* clang-format on */
  auto obj = rapidobj::ParseFile(path);

  if (obj.error) {
    return std::unexpected{std::format(/* clang-format off */
      "Error parsing OBJ File (at line {}):\n\t{}",
      obj.error.line_num,
      util::error::indent(obj.error.code.message())
    )};/* clang-format on */
  }

  rapidobj::Triangulate(obj);

  if (obj.error) {
    return std::unexpected{std::format(/* clang-format off */
      "Error triangulating OBJ File (at line {}):\n\t'{}'",
      obj.error.line_num,
      util::error::indent(obj.error.code.message())
    )};/* clang-format on */
  }

  std::vector<asset::Material> materials;
  for (const auto& material : obj.materials) {
    asset::Material mat = {/* clang-format off */
        .name = material.name,

        .ambient = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]),
        .diffuse = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]),
        .specular = glm::vec3(material.specular[0], material.specular[1], material.specular[2]),
        .shininess = material.shininess,
        .dissolve = material.dissolve
    };/* clang-format on */

    if (!material.diffuse_texname.empty()) {
      auto unresolvedPath = std::filesystem::path(material.diffuse_texname);
      auto resolvedPath = path.parent_path() / unresolvedPath;

      auto asset = asset::loader::Img::tryFromFile(resolvedPath, texMan);
      if (!asset.has_value()) {
        return std::unexpected{std::format("Failed to load texture: {}", asset.error())};
      }

      mat.diffuseTexture = {/* clang-format off */
        .index = asset->textureId,
        .uvScale = glm::vec2(1.0f, 1.0f),
        .uvOffset = glm::vec2(0.0f, 0.0f),
        .uvRotation = 0.0f
      }; /* clang-format on */
    }

    if (!material.normal_texname.empty()) {
      auto unresolvedPath = std::filesystem::path(material.normal_texname);
      auto resolvedPath = path.parent_path() / unresolvedPath;

      auto asset = asset::loader::Img::tryFromFile(resolvedPath, texMan);
      if (!asset.has_value()) {
        return std::unexpected{std::format("Failed to load normal texture: {}", asset.error())};
      }

      mat.normalTexture = {/* clang-format off */
        .index = asset->textureId,
        .uvScale = glm::vec2(1.0f, 1.0f),
        .uvOffset = glm::vec2(0.0f, 0.0f),
        .uvRotation = 0.0f
      }; /* clang-format on */
    }

    materials.push_back(mat);
  }

  std::vector<Vertex3D> vertices;
  std::vector<asset::Shape> shapes;

  for (const auto& shape : obj.shapes) {
    // Create a map of material ID to vector of indices
    std::map<int, std::vector<int>> materialGroups;

    for (size_t i = 0; i < shape.mesh.indices.size(); i++) {
      const auto& index = shape.mesh.indices[i];
      auto vertexIndex = vertices.size();

      /* clang-format off */
      glm::vec3 position = convertFromObj(
        obj.attributes.positions[index.position_index * 3],
        obj.attributes.positions[index.position_index * 3 + 1],
        obj.attributes.positions[index.position_index * 3 + 2]
      );/* clang-format on */

      // Default values for normal and UV in case they're not provided
      glm::vec3 normal = position;
      glm::vec2 uv = glm::vec2(0.0f, 0.0f);

      // Check if normal index is valid and normals array is not empty
      if (index.normal_index >= 0) {
        normal = glm::normalize(convertFromObj(/* clang-format off */
          obj.attributes.normals[index.normal_index * 3],
          obj.attributes.normals[index.normal_index * 3 + 1],
          obj.attributes.normals[index.normal_index * 3 + 2]
        ));/* clang-format on */
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
      int materialId = -1; // Default material ID
      if (faceIndex < static_cast<int>(shape.mesh.material_ids.size())) {
        materialId = shape.mesh.material_ids[faceIndex];
      }

      // Add vertex index to the appropriate material group
      materialGroups[materialId].push_back(vertexIndex);
    }

    // Convert the map to the vector of asset::MaterialGroup
    std::vector<asset::MaterialGroup> groups;
    for (const auto& [materialId, indices] : materialGroups) {
      if (materialId == -1) {
        groups.push_back(asset::MaterialGroup{.materialId = std::nullopt, .indices = indices});
      } else {
        groups.push_back(asset::MaterialGroup{.materialId = materialId, .indices = indices});
      }
    }

    shapes.push_back(asset::Shape{/* clang-format off */
        .name = shape.name,
        .groups = std::move(groups)
    });/* clang-format on */
  }

  return asset::Asset3D{/* clang-format off */
    std::move(vertices),
    std::move(shapes),
    std::move(materials),
    std::move(path)
  };/* clang-format on */
}
