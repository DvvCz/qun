#include "gltf.hpp"

#include <fastgltf/tools.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>

std::expected<asset::Gltf, std::string> asset::Gltf::tryFromFile(const std::filesystem::path& path) noexcept {
  static constexpr auto supportedExtensions = /* clang-format off */
    fastgltf::Extensions::KHR_mesh_quantization |
    fastgltf::Extensions::KHR_texture_transform |
    fastgltf::Extensions::KHR_materials_variants; /* clang-format on */

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

  return Gltf{path};
}
