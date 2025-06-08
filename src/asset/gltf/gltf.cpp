#include "gltf.hpp"

#include <expected>
#include <fastgltf/tools.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <format>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "fastgltf/math.hpp"
#include "render/texture.hpp"

/* clang-format off */
std::expected<asset::Asset3D, std::string> asset::loader::Gltf::tryFromFile(
  const std::filesystem::path& path,
  texture::Manager& texMan
) noexcept { /* clang-format on */
  /* clang-format off */
  auto extensions =
    fastgltf::Extensions::KHR_materials_transmission |
    fastgltf::Extensions::KHR_materials_specular |
    fastgltf::Extensions::KHR_texture_transform |
    fastgltf::Extensions::KHR_materials_unlit;
  /* clang-format on */

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

  auto assetResult = parser.loadGltf(gltfFile.get(), path.parent_path(), gltfOptions);
  if (assetResult.error() != fastgltf::Error::None) {
    return std::unexpected{std::format("Failed to load GLTF file: {}", fastgltf::getErrorMessage(assetResult.error()))};
  }

  auto& asset = assetResult.get();

  auto materialsResult = tryConvertMaterials(asset, texMan);
  if (!materialsResult.has_value()) {
    return std::unexpected{materialsResult.error()};
  }

  std::vector<asset::Material> materials = std::move(materialsResult.value());
  std::vector<Vertex3D> vertices;

  auto defaultScene = asset.defaultScene.value_or(0);
  if (asset.scenes.empty()) {
    return std::unexpected{"GLTF asset has no scenes defined"};
  }

  std::vector<asset::Node> nodes;
  std::vector<size_t> rootNodes;
  for (auto& gltfNodeIdx : asset.scenes[defaultScene].nodeIndices) {
    auto& gltfNode = asset.nodes[gltfNodeIdx];

    auto nodeResult = Gltf::tryConvertNode(asset, gltfNode, fastgltf::math::fmat4x4(1.0f), nodes, vertices);
    if (!nodeResult.has_value()) {
      return std::unexpected{nodeResult.error()};
    }

    rootNodes.push_back(nodes.size());
    nodes.push_back(std::move(nodeResult.value()));
  }

  return asset::Asset3D{/* clang-format off */
    std::move(vertices),
    std::move(nodes),
    std::move(rootNodes),
    std::move(materials),
    path
  };/* clang-format on */
}
