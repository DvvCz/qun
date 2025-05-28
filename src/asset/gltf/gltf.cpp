#include "gltf.hpp"

#include <expected>
#include <fastgltf/tools.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <map>
#include <format>
#include <print>
#include <variant>

#include "asset/img/img.hpp"

#include "render/texture.hpp"

#include "constants.hpp"

static glm::vec3 convertFromGLTF(float x, float y, float z) noexcept {
  return glm::vec3(x, -z, y);
};

/* clang-format off */
static std::expected<size_t, std::string> tryCreateTexture(
  const fastgltf::Asset& asset,
  const fastgltf::Image& image,
  texture::Manager& texMan
) { /* clang-format on */
  if (std::holds_alternative<fastgltf::sources::URI>(image.data)) {
    auto& uriData = std::get<fastgltf::sources::URI>(image.data);
    return std::unexpected("todo: uri texture loading not implemented yet");
  }

  if (std::holds_alternative<fastgltf::sources::BufferView>(image.data)) {
    auto& bufferData = std::get<fastgltf::sources::BufferView>(image.data);
    auto& bufferView = asset.bufferViews[bufferData.bufferViewIndex];

    auto& buffer = asset.buffers[bufferView.bufferIndex];
    if (!std::holds_alternative<fastgltf::sources::Array>(buffer.data)) {
      return std::unexpected("Buffer data is not an array, cannot load texture");
    }

    auto& arrayData = std::get<fastgltf::sources::Array>(buffer.data);

    switch (bufferData.mimeType) {
    case fastgltf::MimeType::PNG:
    case fastgltf::MimeType::JPEG: {
      auto dataStart = arrayData.bytes.cbegin() + bufferView.byteOffset;
      auto dataEnd = dataStart + bufferView.byteLength;
      std::vector<std::byte> imageData(dataStart, dataEnd);

      auto out = asset::loader::Img::tryFromData(imageData, texMan);
      if (!out.has_value()) {
        return std::unexpected{out.error()};
      }

      return out.value().textureId;
    }
    default:
      return std::unexpected{
          std::format("Unsupported texture mime type: {}", fastgltf::getMimeTypeString(bufferData.mimeType))};
    }
  }

  if (std::holds_alternative<fastgltf::sources::Array>(image.data)) {
    auto& arrayData = std::get<fastgltf::sources::Array>(image.data);

    std::vector<std::byte> imageData(arrayData.bytes.cbegin(), arrayData.bytes.cend());

    auto out = asset::loader::Img::tryFromData(imageData, texMan);
    if (!out.has_value()) {
      return std::unexpected{out.error()};
    }

    return out.value().textureId;
  }

  return std::unexpected{"Unknown buffer data type for texture"};
}

/* clang-format off */
std::expected<asset::Asset3D, std::string> asset::loader::Gltf::tryFromFile(
  const std::filesystem::path& path,
  texture::Manager& texMan
) noexcept { /* clang-format on */
  auto extensions = fastgltf::Extensions::KHR_materials_transmission;

  fastgltf::Parser parser(extensions);

  auto gltfFile = fastgltf::MappedGltfFile::FromPath(path);
  if (!bool(gltfFile)) {
    return std::unexpected{std::format("Failed to map GLTF file: {}", fastgltf::getErrorMessage(gltfFile.error()))};
  }

  constexpr auto gltfOptions = /* clang-format off */
    fastgltf::Options::DontRequireValidAssetMember |
    fastgltf::Options::AllowDouble |
    fastgltf::Options::LoadExternalBuffers |
    fastgltf::Options::LoadExternalImages |
    fastgltf::Options::GenerateMeshIndices; /* clang-format on */

  auto rawAsset = parser.loadGltf(gltfFile.get(), path.parent_path(), gltfOptions);

  if (rawAsset.error() != fastgltf::Error::None) {
    return std::unexpected{std::format("Failed to load GLTF file: {}", fastgltf::getErrorMessage(rawAsset.error()))};
  }

  auto& asset = rawAsset.get();

  // Convert fastgltf::Asset to asset::Asset3D
  std::vector<asset::Material> materials;
  std::vector<Vertex3D> vertices;
  std::vector<asset::Shape> shapes;

  // Convert materials
  for (const auto& gltfMaterial : asset.materials) {
    glm::vec3 baseColorFactor = glm::vec3(/* clang-format off */
      gltfMaterial.pbrData.baseColorFactor[0],
      gltfMaterial.pbrData.baseColorFactor[1],
      gltfMaterial.pbrData.baseColorFactor[2]
    ); /* clang-format on */
    float baseColorAlpha = gltfMaterial.pbrData.baseColorFactor[3];

    // These were manually picked to try and replicate phong properties
    // There's likely a better way to do this, but works for now.
    asset::Material mat = {/* clang-format off */
        .name = std::string(gltfMaterial.name),
        .ambient = baseColorFactor * 0.2f,
        .diffuse = baseColorFactor * 0.4f,
        .specular = glm::vec3(0.5f),
        .shininess = std::max(1.0f, 1 / std::pow(gltfMaterial.pbrData.roughnessFactor, 2.0f)),
        .dissolve = baseColorAlpha,
        .diffuseTexture = std::nullopt,
        .normalTexture = std::nullopt
    };/* clang-format on */

    auto getTexture = [&asset, &texMan](size_t textureIndex) -> std::expected<size_t, std::string> {
      if (textureIndex >= asset.textures.size()) {
        return std::unexpected{"Texture index out of bounds"};
      }

      auto& texture = asset.textures[textureIndex];
      if (!texture.imageIndex.has_value() || texture.imageIndex.value() >= asset.images.size()) {
        return std::unexpected{"Invalid image index for texture"};
      }

      auto& image = asset.images[texture.imageIndex.value()];
      return tryCreateTexture(asset, image, texMan);
    };

    if (gltfMaterial.normalTexture.has_value()) {
      auto out = getTexture(gltfMaterial.normalTexture.value().textureIndex);
      if (!out.has_value()) {
        return std::unexpected{out.error()};
      }

      mat.normalTexture = out.value();
    }

    if (gltfMaterial.pbrData.baseColorTexture.has_value()) {
      auto out = getTexture(gltfMaterial.pbrData.baseColorTexture.value().textureIndex);
      if (!out.has_value()) {
        return std::unexpected{out.error()};
      }

      mat.diffuseTexture = out.value();
    }

    materials.push_back(mat);
  }

  // Process meshes
  for (const auto& gltfMesh : asset.meshes) {
    // Create a map of material ID to vector of indices
    std::map<int, std::vector<int>> materialGroups;

    for (const auto& primitive : gltfMesh.primitives) {
      // Find position attribute
      auto* positionIt = primitive.findAttribute("POSITION");
      if (positionIt == primitive.attributes.end()) {
        continue; // Skip primitives without positions
      }

      auto& positionAccessor = asset.accessors[positionIt->accessorIndex];
      if (!positionAccessor.bufferViewIndex.has_value()) {
        continue;
      }

      // Find normal attribute
      auto* normalIt = primitive.findAttribute("NORMAL");

      // Find tangent attribute
      auto* tangentIt = primitive.findAttribute("TANGENT");

      // Find texture coordinate attribute
      auto* texcoordIt = primitive.findAttribute("TEXCOORD_0");

      // Extract vertex data
      std::vector<Vertex3D> primitiveVertices;
      primitiveVertices.resize(positionAccessor.count);

      // Extract positions
      fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
          asset, positionAccessor, [&](fastgltf::math::fvec3 pos, std::size_t idx) {
            primitiveVertices[idx].pos = convertFromGLTF(pos.x(), pos.y(), pos.z());
          });

      // Extract normals if available
      if (normalIt != primitive.attributes.end()) {
        auto& normalAccessor = asset.accessors[normalIt->accessorIndex];
        if (normalAccessor.bufferViewIndex.has_value()) {
          fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
              asset, normalAccessor, [&](fastgltf::math::fvec3 normal, std::size_t idx) {
                primitiveVertices[idx].normal = glm::normalize(convertFromGLTF(normal.x(), normal.y(), normal.z()));
              });
        }
      } else {
        for (auto& vertex : primitiveVertices) {
          vertex.normal = constants::WORLD_UP;
        }
      }

      // Extract tangents if available
      if (tangentIt != primitive.attributes.end()) {
        auto& tangentAccessor = asset.accessors[tangentIt->accessorIndex];
        if (tangentAccessor.bufferViewIndex.has_value()) {
          fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(
              asset, tangentAccessor, [&](fastgltf::math::fvec4 tangent, std::size_t idx) {
                primitiveVertices[idx].tangent = glm::normalize(convertFromGLTF(tangent.x(), tangent.y(), tangent.z()));
              });
        }
      } else {
        // Generate default tangents (pointing along X-axis in Z-up coordinate system)
        for (auto& vertex : primitiveVertices) {
          vertex.tangent = constants::WORLD_FORWARD;
        }
      }

      // Extract texture coordinates if available
      if (texcoordIt != primitive.attributes.end()) {
        auto& texcoordAccessor = asset.accessors[texcoordIt->accessorIndex];
        if (texcoordAccessor.bufferViewIndex.has_value()) {
          fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
              asset, texcoordAccessor,
              [&](fastgltf::math::fvec2 uv, std::size_t idx) { primitiveVertices[idx].uv = glm::vec2(uv.x(), uv.y()); });
        }
      } else {
        for (auto& vertex : primitiveVertices) {
          vertex.uv = glm::vec2(0.0f, 0.0f);
        }
      }

      int materialId = -1;
      if (primitive.materialIndex.has_value()) {
        materialId = primitive.materialIndex.value();
      }

      // Handle indices
      if (primitive.indicesAccessor.has_value()) {
        auto& indexAccessor = asset.accessors[primitive.indicesAccessor.value()];

        // Extract indices and create vertices
        std::vector<std::uint32_t> indices;
        indices.resize(indexAccessor.count);

        fastgltf::iterateAccessorWithIndex<std::uint32_t>(asset, indexAccessor,
                                                          [&](std::uint32_t index, std::size_t idx) { indices[idx] = index; });

        // Add vertices to the global vertex array and record their indices
        for (std::uint32_t index : indices) {
          if (index < primitiveVertices.size()) {
            auto vertexIndex = vertices.size();
            vertices.push_back(primitiveVertices[index]);
            materialGroups[materialId].push_back(static_cast<int>(vertexIndex));
          }
        }
      } else {
        // No indices, add vertices directly
        for (const auto& vertex : primitiveVertices) {
          auto vertexIndex = vertices.size();
          vertices.push_back(vertex);
          materialGroups[materialId].push_back(static_cast<int>(vertexIndex));
        }
      }
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
        .name = std::string(gltfMesh.name),
        .groups = std::move(groups)
    });/* clang-format on */
  }

  return asset::Asset3D{/* clang-format off */
    std::move(vertices),
    std::move(shapes),
    std::move(materials),
    path
  };/* clang-format on */
}
