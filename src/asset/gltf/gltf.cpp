#include "gltf.hpp"

#include <expected>
#include <fastgltf/tools.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <format>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "render/texture.hpp"

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

  std::vector<asset::Shape> shapes;
  std::string errorMessage;
  bool hasError = false;

  fastgltf::iterateSceneNodes(/* clang-format off */
    asset,
    defaultScene,
    fastgltf::math::fmat4x4(),
    [&](fastgltf::Node& node, fastgltf::math::fmat4x4 worldTransformRaw) {
      if (hasError) return;

      if (!node.meshIndex.has_value()) {
        // Nothing to render for this node.
        // Still important for transforms / grouping.
        return;
      }

      const auto& mesh = asset.meshes[node.meshIndex.value()];

      // todo: revert terrible conversion name change
      auto worldTransform = Gltf::parserMatAsGlm(worldTransformRaw);
      auto shapeResult = Gltf::tryConvertNode(asset, node, mesh, worldTransform, vertices);

      if (!shapeResult.has_value()) {
        errorMessage = shapeResult.error();
        hasError = true;
        return;
      }

      shapes.push_back(std::move(shapeResult.value()));
    }
  ); /* clang-format on */

  if (hasError) {
    return std::unexpected{errorMessage};
  }

  return asset::Asset3D{/* clang-format off */
    std::move(vertices),
    std::move(shapes),
    std::move(materials),
    path
  };/* clang-format on */
}
