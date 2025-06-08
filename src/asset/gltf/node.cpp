#include "asset/asset.hpp"
#include "constants.hpp"
#include "fastgltf/math.hpp"
#include "fastgltf/tools.hpp"
#include "gltf.hpp"
#include <map>
#include <variant>

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
    if (std::abs(denominator) > 1e-6f) {
      float f = 1.0f / denominator;
      tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
    } else {
      // Fallback: generate tangent perpendicular to normal
      glm::vec3 normal = v0.normal;
      glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
      if (std::abs(glm::dot(normal, up)) > 0.9f) {
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
      if (std::abs(glm::dot(normal, up)) > 0.9f) {
        up = glm::vec3(1.0f, 0.0f, 0.0f);
      }
      vertex.tangent = glm::normalize(glm::cross(up, normal));
    }
  }
}

/* clang-format off */
std::expected<asset::Node, std::string> asset::loader::Gltf::tryConvertNode(
  const fastgltf::Asset& asset,
  const fastgltf::Node& node,
  const fastgltf::math::fmat4x4& gltfParentTransform,
  std::vector<Node>& allNodes,
  std::vector<Vertex3D>& allVertices
) noexcept { /* clang-format off */
  asset::Node outputNode;
  outputNode.name = std::string(node.name.empty() ? "<unnamed>" : node.name);

  auto gltfNodeTransform = fastgltf::getTransformMatrix(node, gltfParentTransform);
  for (auto& childIdx: node.children) {
    auto childNode = asset.nodes[childIdx];

    auto childResult = tryConvertNode(asset, childNode, gltfNodeTransform, allNodes, allVertices);
    if (!childResult.has_value()) {
      return std::unexpected{childResult.error()};
    }

    outputNode.children.push_back(allNodes.size());
    allNodes.push_back(std::move(childResult.value()));
  }

  if (!node.meshIndex.has_value()) {
    return outputNode; // No mesh, just a transform node
  }
  const auto& mesh = asset.meshes[node.meshIndex.value()];

  auto rawNodeTransform = asset::loader::Gltf::parserMatAsGlm(gltfNodeTransform);
  auto rawNormalMatrix = glm::transpose(glm::inverse(glm::mat3(rawNodeTransform)));

  std::map<int, std::vector<int>> materialGroups;
  for (const auto& primitive : mesh.primitives) {
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
          auto raw = Gltf::parserVecAsGlm(pos);
          auto rawTransform = glm::vec3(rawNodeTransform * glm::vec4(raw, 1.0f));
          primitiveVertices[idx].pos = convertFromGLTF(rawTransform.x, rawTransform.y, rawTransform.z);
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
          auto raw = Gltf::parserVecAsGlm(normal);
          auto rawWorldNormal = rawNormalMatrix * raw;
          auto worldNormal = convertFromGLTF(rawWorldNormal.x, rawWorldNormal.y, rawWorldNormal.z);
          primitiveVertices[idx].normal = glm::normalize(worldNormal);
        }
      ); /* clang-format on */
    } else {
      for (auto& vertex : primitiveVertices) {
        vertex.normal = constants::WORLD_UP;
      }
    }

    // UVs
    {
      auto& material = asset.materials[materialId];
      size_t uvSetIdx = 0;

      if (material.pbrData.baseColorTexture.has_value()) {
        auto& baseColorTexture = material.pbrData.baseColorTexture.value();

        uvSetIdx = baseColorTexture.texCoordIndex;

        if (baseColorTexture.transform) {
          // Handle texture transform if it exists
          auto& transform = *baseColorTexture.transform;

          if (transform.texCoordIndex.has_value()) {
            uvSetIdx = transform.texCoordIndex.value();
          }
        }
      }

      auto* uvIterator = primitive.findAttribute("TEXCOORD_" + std::to_string(uvSetIdx));
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
          auto raw = Gltf::parserVecAsGlm(tangent);
          auto rawWorldTangent = rawNormalMatrix * raw;
          auto worldTangent = convertFromGLTF(rawWorldTangent.x, rawWorldTangent.y, rawWorldTangent.z);
          primitiveVertices[idx].tangent = glm::normalize(worldTangent);
        }
      ); /* clang-format on */
    } else {
      calculateTangents(primitiveVertices, indices);
    }

    // Add vertices to the global vertex array and record their indices
    for (std::uint32_t index : indices) {
      auto vertexIndex = allVertices.size();
      allVertices.push_back(primitiveVertices[index]);
      materialGroups[materialId].push_back(static_cast<int>(vertexIndex));
    }
  }

  for (const auto& [materialId, indices] : materialGroups) {
    if (materialId == -1) {
      outputNode.groups.push_back(asset::MaterialGroup{.materialId = std::nullopt, .indices = indices});
    } else {
      outputNode.groups.push_back(asset::MaterialGroup{.materialId = materialId, .indices = indices});
    }
  }

  return outputNode;
}
