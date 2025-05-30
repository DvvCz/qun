#include "gltf.hpp"

#include <expected>
#include <fastgltf/tools.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <map>
#include <format>
#include <print>
#include <variant>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "asset/img/img.hpp"

#include "render/texture.hpp"

#include "constants.hpp"

static glm::vec3 convertFromGLTF(float x, float y, float z) noexcept {
  return glm::vec3(z, -x, y);
};

static glm::vec3 glmVecFromParserVec(const fastgltf::math::nvec3& vec) noexcept {
  return glm::vec3(vec[0], vec[1], vec[2]);
}

static glm::vec3 glmVecFromParserVec(const fastgltf::math::nvec4& vec) noexcept {
  return glm::vec3(vec[0], vec[1], vec[2]);
}

static glm::mat4x4 glmMatFromParserMat(const fastgltf::math::fmat4x4& mat) noexcept {
  return glm::mat4(/* clang-format off */
    mat[0][0], mat[0][1], mat[0][2], mat[0][3],
    mat[1][0], mat[1][1], mat[1][2], mat[1][3],
    mat[2][0], mat[2][1], mat[2][2], mat[2][3],
    mat[3][0], mat[3][1], mat[3][2], mat[3][3]
  );/* clang-format on */
}

static glm::quat glmQuatFromParserQuat(const fastgltf::math::fquat& quat) noexcept {
  // fastgltf uses w, x, y, z order for quaternions
  return glm::quat(quat[3], quat[0], quat[1], quat[2]);
}

glm::mat4 matConvertFromGLTF = glm::mat4(/* clang-format off */
  0.0f,  0.0f,  1.0f,  0.0f,  // X' = Z (glTF forward becomes your forward)
  -1.0f, 0.0f,  0.0f,  0.0f,  // Y' = -X (glTF -right becomes your right)
  0.0f,  1.0f,  0.0f,  0.0f,  // Z' = Y (glTF up becomes your up)
  0.0f,  0.0f,  0.0f,  1.0f   // W' = W (homogeneous coordinate)
); /* clang-format on */

static glm::mat4 getNodeTransform(const fastgltf::Node& node) noexcept {
  if (std::holds_alternative<fastgltf::math::fmat4x4>(node.transform)) {
    auto gltfMat = glmMatFromParserMat(std::get<fastgltf::math::fmat4x4>(node.transform));
    return gltfMat * matConvertFromGLTF;
  } else {
    auto& trs = std::get<fastgltf::TRS>(node.transform);

    auto translation = glmVecFromParserVec(trs.translation);
    auto rotation = glmQuatFromParserQuat(trs.rotation);
    auto scale = glmVecFromParserVec(trs.scale);

    auto gltfMat = glm::translate(glm::mat4(1.0f), translation) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale);
    return gltfMat * matConvertFromGLTF;
  }
}

// Traverse node hierarchy and apply transforms
static glm::mat4 calculateWorldTransform(/* clang-format off */
  const fastgltf::Asset& asset,
  size_t nodeIndex,
  const std::vector<glm::mat4>& nodeTransforms
) noexcept { /* clang-format on */
  auto currentTransform = nodeTransforms[nodeIndex];

  // Find parent of this node
  for (size_t i = 0; i < asset.nodes.size(); ++i) {
    const auto& node = asset.nodes[i];
    for (size_t childIndex : node.children) {
      if (childIndex == nodeIndex) {
        // Found parent, recursively calculate its world transform
        return calculateWorldTransform(asset, i, nodeTransforms) * currentTransform;
      }
    }
  }

  // No parent found, this is a root node
  return currentTransform;
}

// Calculate tangents for a set of vertices using Lengyel's method
static void calculateTangents(std::vector<Vertex3D>& vertices, const std::vector<std::uint32_t>& indices) {
  for (auto& vertex : vertices) {
    vertex.tangent = glm::vec3(0.0f);
  }

  // Calculate tangents for each triangle
  for (size_t i = 0; i < indices.size(); i += 3) {
    if (i + 2 >= indices.size()) {
      break;
    }

    std::uint32_t i0 = indices[i];
    std::uint32_t i1 = indices[i + 1];
    std::uint32_t i2 = indices[i + 2];

    if (i0 >= vertices.size() || i1 >= vertices.size() || i2 >= vertices.size()) {
      continue;
    }

    Vertex3D& v0 = vertices[i0];
    Vertex3D& v1 = vertices[i1];
    Vertex3D& v2 = vertices[i2];

    // Calculate edge vectors
    glm::vec3 edge1 = v1.pos - v0.pos;
    glm::vec3 edge2 = v2.pos - v0.pos;

    // Calculate UV deltas
    glm::vec2 deltaUV1 = v1.uv - v0.uv;
    glm::vec2 deltaUV2 = v2.uv - v0.uv;

    // Calculate tangent using cross product method
    float denominator = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;

    glm::vec3 tangent;
    if (abs(denominator) > 1e-6f) {
      float f = 1.0f / denominator;
      tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
    } else {
      // Fallback: generate tangent perpendicular to normal
      glm::vec3 normal = v0.normal;
      glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
      if (abs(glm::dot(normal, up)) > 0.9f) {
        up = glm::vec3(1.0f, 0.0f, 0.0f);
      }
      tangent = glm::normalize(glm::cross(up, normal));
    }

    // Accumulate tangents for each vertex of the triangle
    v0.tangent += tangent;
    v1.tangent += tangent;
    v2.tangent += tangent;
  }

  // Normalize and orthogonalize tangents using Gram-Schmidt process
  for (auto& vertex : vertices) {
    if (glm::length(vertex.tangent) > 1e-6f) {
      // Orthogonalize tangent with respect to normal (Gram-Schmidt)
      vertex.tangent = glm::normalize(vertex.tangent - glm::dot(vertex.tangent, vertex.normal) * vertex.normal);
    } else {
      // Fallback: generate tangent perpendicular to normal
      glm::vec3 normal = vertex.normal;
      glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
      if (abs(glm::dot(normal, up)) > 0.9f) {
        up = glm::vec3(1.0f, 0.0f, 0.0f);
      }
      vertex.tangent = glm::normalize(glm::cross(up, normal));
    }
  }
}

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
  auto extensions = fastgltf::Extensions::KHR_materials_transmission | fastgltf::Extensions::KHR_materials_specular;

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
    // Base physically based data
    auto& pbrInfo = gltfMaterial.pbrData;

    // KHR_materials_specular
    auto& specularInfo = gltfMaterial.specular;

    glm::vec3 baseColor = glmVecFromParserVec(pbrInfo.baseColorFactor);
    float baseAlpha = pbrInfo.baseColorFactor[3];

    glm::vec3 specular;
    if (gltfMaterial.specular) {
      specular = glmVecFromParserVec(specularInfo->specularColorFactor) * specularInfo->specularFactor;
    } else {
      specular = baseColor * (pbrInfo.metallicFactor - pbrInfo.roughnessFactor * 0.5f);
    }

    // Rough manual translations from PBR to phong.
    asset::Material mat = {/* clang-format off */
        .name = std::string(gltfMaterial.name),
        .ambient = baseColor * 0.2f,
        .diffuse = (1.0f - pbrInfo.metallicFactor) * baseColor * 0.6f,
        .specular = specular,
        .shininess = std::max(1.0f, 1 / std::pow(pbrInfo.roughnessFactor, 2.0f)),
        .dissolve = baseAlpha,
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
  // First, calculate transforms for all nodes
  std::vector<glm::mat4> nodeTransforms;
  nodeTransforms.reserve(asset.nodes.size());

  for (const auto& node : asset.nodes) {
    nodeTransforms.push_back(getNodeTransform(node));
  }

  // Calculate world transforms for each node
  std::vector<glm::mat4> worldTransforms;
  worldTransforms.reserve(asset.nodes.size());

  for (size_t i = 0; i < asset.nodes.size(); ++i) {
    worldTransforms.push_back(calculateWorldTransform(asset, i, nodeTransforms));
  }

  // Process nodes that reference meshes
  for (size_t nodeIndex = 0; nodeIndex < asset.nodes.size(); ++nodeIndex) {
    const auto& node = asset.nodes[nodeIndex];

    if (!node.meshIndex.has_value()) {
      continue; // Node doesn't reference a mesh
    }

    const auto& gltfMesh = asset.meshes[node.meshIndex.value()];
    const glm::mat4& worldTransform = worldTransforms[nodeIndex];

    // Extract normal matrix (for transforming normals and tangents)
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(worldTransform)));

    // Create a map of material ID to vector of indices
    std::map<int, std::vector<int>> materialGroups;

    for (const auto& primitive : gltfMesh.primitives) {
      int materialId = primitive.materialIndex.value_or(-1);

      std::vector<Vertex3D> primitiveVertices;

      // Positions
      auto* positionIterator = primitive.findAttribute("POSITION");
      if (positionIterator == primitive.attributes.end()) {
        return std::unexpected{"Primitive does not have a POSITION attribute"};
      }
      auto& positionAccessor = asset.accessors[positionIterator->accessorIndex];
      if (!positionAccessor.bufferViewIndex.has_value()) {
        return std::unexpected{"Position accessor does not have a valid buffer view"};
      }
      primitiveVertices.resize(positionAccessor.count);
      fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(/* clang-format off */
        asset,
        positionAccessor,
        [&](fastgltf::math::fvec3 pos, std::size_t idx) {
          glm::vec3 localPos = convertFromGLTF(pos.x(), pos.y(), pos.z());
          glm::vec4 worldPos = worldTransform * glm::vec4(localPos, 1.0f);
          primitiveVertices[idx].pos = glm::vec3(worldPos);
        }
      ); /* clang-format on */

      // Indices
      std::vector<std::uint32_t> indices;
      if (primitive.indicesAccessor.has_value()) { // Has indices
        auto& indexAccessor = asset.accessors[primitive.indicesAccessor.value()];
        indices.resize(indexAccessor.count);

        /* clang-format off */
        fastgltf::iterateAccessorWithIndex<std::uint32_t>(
          asset,
          indexAccessor,
          [&](std::uint32_t index, std::size_t idx) {
            indices[idx] = index;
          }
        ); /* clang-format on */
      } else {
        indices.resize(primitiveVertices.size());

        for (size_t i = 0; i < primitiveVertices.size(); ++i) {
          indices[i] = static_cast<std::uint32_t>(i);
        }
      }

      // Normals
      auto* normalIterator = primitive.findAttribute("NORMAL");
      if (normalIterator != primitive.attributes.end()) { // Has normals
        auto& normalAccessor = asset.accessors[normalIterator->accessorIndex];
        if (!normalAccessor.bufferViewIndex.has_value()) {
          return std::unexpected{"Normal accessor does not have a valid buffer view"};
        }

        /* clang-format off */
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
          asset,
          normalAccessor,
          [&](fastgltf::math::fvec3 normal, std::size_t idx) {
            glm::vec3 localNormal = convertFromGLTF(normal.x(), normal.y(), normal.z());
            glm::vec3 worldNormal = normalMatrix * localNormal;
            primitiveVertices[idx].normal = glm::normalize(worldNormal);
          }
        ); /* clang-format on */
      } else {
        for (auto& vertex : primitiveVertices) {
          vertex.normal = constants::WORLD_UP;
        }
      }

      // UVs
      auto* uvIterator = primitive.findAttribute("TEXCOORD_0");
      if (uvIterator != primitive.attributes.end()) {
        auto& texcoordAccessor = asset.accessors[uvIterator->accessorIndex];
        if (!texcoordAccessor.bufferViewIndex.has_value()) {
          return std::unexpected{"Texture coordinate accessor does not have a valid buffer view"};
        }

        /* clang-format off */
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
            asset,
            texcoordAccessor,
            [&](fastgltf::math::fvec2 uv, std::size_t idx) {
              primitiveVertices[idx].uv = glm::vec2(uv.x(), uv.y());
            }
        ); /* clang-format on */
      } else {
        for (auto& vertex : primitiveVertices) {
          vertex.uv = glm::vec2(0.0f, 0.0f);
        }
      }

      // Tangents
      auto* tangentIterator = primitive.findAttribute("TANGENT");
      if (tangentIterator != primitive.attributes.end()) {
        auto& tangentAccessor = asset.accessors[tangentIterator->accessorIndex];
        if (!tangentAccessor.bufferViewIndex.has_value()) {
          return std::unexpected{"Tangent accessor does not have a valid buffer view"};
        }

        /* clang-format off */
        fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec4>(
          asset,
          tangentAccessor,
          [&](fastgltf::math::fvec4 tangent, std::size_t idx) {
            glm::vec3 localTangent = convertFromGLTF(tangent.x(), tangent.y(), tangent.z());
            glm::vec3 worldTangent = normalMatrix * localTangent;
            primitiveVertices[idx].tangent = glm::normalize(worldTangent);
          }
        ); /* clang-format on */
      } else {
        calculateTangents(primitiveVertices, indices);
      }

      // Add vertices to the global vertex array and record their indices
      for (std::uint32_t index : indices) {
        auto vertexIndex = vertices.size();
        vertices.push_back(primitiveVertices[index]);
        materialGroups[materialId].push_back(static_cast<int>(vertexIndex));
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

    // Create shape name from node name or mesh name
    std::string shapeName;
    if (!node.name.empty()) {
      shapeName = std::string(node.name);
    } else if (!gltfMesh.name.empty()) {
      shapeName = std::string(gltfMesh.name);
    } else {
      shapeName = std::format("Shape_{}", nodeIndex);
    }

    shapes.push_back(asset::Shape{/* clang-format off */
      .name = shapeName,
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
