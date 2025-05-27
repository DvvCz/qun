#include "gltf.hpp"

#include <fastgltf/tools.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <map>
#include <format>
#include <print>
#include <variant>

#include "asset/img/img.hpp"

#include "render/texture.hpp"

/* clang-format off */
std::expected<asset::Asset3D, std::string> asset::loader::Gltf::tryFromFile(
  const std::filesystem::path& path,
  texture::Manager& texMan
) noexcept { /* clang-format on */
  /* clang-format off */
  static constexpr auto supportedExtensions =
    fastgltf::Extensions::KHR_mesh_quantization  |
    fastgltf::Extensions::KHR_texture_transform  |
    fastgltf::Extensions::KHR_materials_variants |
    fastgltf::Extensions::KHR_texture_basisu;
  /* clang-format on */

  fastgltf::Parser parser(supportedExtensions);

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
        .diffuse = baseColorFactor * 0.8f,
        .specular = glm::vec3(0.5f),
        .shininess = std::max(1.0f, 1 / std::pow(gltfMaterial.pbrData.roughnessFactor, 2.0f)),
        .dissolve = baseColorAlpha,
        .diffuseTexture = std::nullopt
    };/* clang-format on */

    if (gltfMaterial.pbrData.baseColorTexture.has_value()) {
      auto& baseColorTexture = gltfMaterial.pbrData.baseColorTexture.value();

      if (baseColorTexture.textureIndex >= asset.textures.size()) {
        std::println("Base color texture index out of bounds, skipping texture");
        continue;
      }

      auto& texture = asset.textures[baseColorTexture.textureIndex];
      if (!texture.imageIndex.has_value() || texture.imageIndex.value() >= asset.images.size()) {
        std::println("Invalid image index for texture, skipping texture");
        continue;
      }

      auto& image = asset.images[texture.imageIndex.value()];

      if (std::holds_alternative<fastgltf::sources::URI>(image.data)) {
        auto& uriData = std::get<fastgltf::sources::URI>(image.data);
        std::println("TODO: uri data for texture: {}", uriData.uri.path());
      } else if (std::holds_alternative<fastgltf::sources::BufferView>(image.data)) {
        auto& bufferData = std::get<fastgltf::sources::BufferView>(image.data);
        auto& bufferView = asset.bufferViews[bufferData.bufferViewIndex];

        auto& buffer = asset.buffers[bufferView.bufferIndex];
        if (!std::holds_alternative<fastgltf::sources::Array>(buffer.data)) {
          std::println("Buffer data is not an array, cannot load texture");
          continue;
        }

        auto& arrayData = std::get<fastgltf::sources::Array>(buffer.data);

        switch (bufferData.mimeType) {
        case fastgltf::MimeType::PNG:
        case fastgltf::MimeType::JPEG: {
          std::vector<std::byte> imageData(arrayData.bytes.cbegin(), arrayData.bytes.cend());

          auto out = asset::loader::Img::tryFromData(imageData, texture::Format::RGBA, texMan);
          if (!out.has_value()) {
            return std::unexpected{out.error()};
          }

          std::println("Loaded texture {}", fastgltf::getMimeTypeString(bufferData.mimeType));

          mat.diffuseTexture = out.value().textureId;
          break;
        }
        default:
          std::println("Unsupported buffer data type for texture {}", fastgltf::getMimeTypeString(bufferData.mimeType));
          break;
        }
      } else {
        std::println("Unknown buffer data type for texture");
      }
    }

    materials.push_back(mat);
  }

  // Process meshes
  for (const auto& gltfMesh : asset.meshes) {
    // Create a map of material ID to vector of indices
    std::map<size_t, std::vector<int>> materialGroups;

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

      // Find texture coordinate attribute
      auto* texcoordIt = primitive.findAttribute("TEXCOORD_0");

      // Extract vertex data
      std::vector<Vertex3D> primitiveVertices;
      primitiveVertices.resize(positionAccessor.count);

      // Extract positions
      fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
          asset, positionAccessor, [&](fastgltf::math::fvec3 pos, std::size_t idx) {
            // Convert from glTF Y-up to renderer Z-up coordinate system
            primitiveVertices[idx].pos = glm::vec3(pos.x(), pos.z(), -pos.y());
          });

      // Extract normals if available
      if (normalIt != primitive.attributes.end()) {
        auto& normalAccessor = asset.accessors[normalIt->accessorIndex];
        if (normalAccessor.bufferViewIndex.has_value()) {
          fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
              asset, normalAccessor, [&](fastgltf::math::fvec3 normal, std::size_t idx) {
                // Convert from glTF Y-up to renderer Z-up coordinate system
                glm::vec3 converted_normal = glm::vec3(normal.x(), normal.z(), -normal.y());
                // Normalize to ensure unit length after coordinate transformation
                primitiveVertices[idx].normal = glm::normalize(converted_normal);
              });
        }
      } else {
        // Generate default normals (pointing up in Z-up coordinate system)
        for (auto& vertex : primitiveVertices) {
          vertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);
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
        // Set default UVs
        for (auto& vertex : primitiveVertices) {
          vertex.uv = glm::vec2(0.0f, 0.0f);
        }
      }

      // Get material ID
      int materialId = 0;
      if (primitive.materialIndex.has_value()) {
        materialId = static_cast<int>(primitive.materialIndex.value());
        if (materialId >= static_cast<int>(materials.size())) {
          materialId = 0; // Fallback to default material
        }
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
      groups.push_back(asset::MaterialGroup{.materialId = materialId, .indices = indices});
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
