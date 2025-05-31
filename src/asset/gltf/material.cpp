#include "gltf.hpp"

/* clang-format off */
std::expected<std::vector<asset::Material>, std::string> asset::loader::Gltf::tryConvertMaterials(
  const fastgltf::Asset& asset,
  texture::Manager& texMan
) noexcept { /* clang-format on */
  std::vector<asset::Material> materials;

  for (const auto& gltfMaterial : asset.materials) {
    // Base physically based data
    auto& pbrInfo = gltfMaterial.pbrData;

    // KHR_materials_specular
    auto& specularInfo = gltfMaterial.specular;

    glm::vec3 baseColor = Gltf::parserVecAsGlm(pbrInfo.baseColorFactor);
    float baseAlpha = pbrInfo.baseColorFactor[3];

    glm::vec3 specular;
    if (gltfMaterial.specular) {
      specular = Gltf::parserVecAsGlm(specularInfo->specularColorFactor) * specularInfo->specularFactor;
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
      return Gltf::tryCreateTexture(asset, image, texMan);
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

  return std::move(materials);
};
