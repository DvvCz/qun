#include "gltf.hpp"

/* clang-format off */
std::expected<std::vector<asset::Material>, std::string> asset::loader::Gltf::tryConvertMaterials(
  const fastgltf::Asset& asset,
  texture::Manager& texMan
) noexcept { /* clang-format on */
  std::vector<asset::Material> materials;

  for (const auto& gltfMaterial : asset.materials) {
    // Physically based data
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
        .dissolve = baseAlpha
    };/* clang-format on */

    auto getTexture = [&asset, &texMan](size_t textureIndex) -> std::expected<texture::Texture, std::string> {
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

    // Has a normal map
    if (gltfMaterial.normalTexture.has_value()) {
      auto& normalTextureInfo = gltfMaterial.normalTexture.value();

      auto normalTextureResult = getTexture(gltfMaterial.normalTexture.value().textureIndex);
      if (!normalTextureResult.has_value()) {
        return std::unexpected{normalTextureResult.error()};
      }

      auto normalTexture = normalTextureResult.value();

      if (normalTextureInfo.transform) {
        auto& transform = *normalTextureInfo.transform;

        normalTexture.uvOffset = glm::vec2(transform.uvOffset[0], transform.uvOffset[1]);
        normalTexture.uvScale *= glm::vec2(transform.uvScale[0], transform.uvScale[1]);
        normalTexture.uvRotation = transform.rotation;
      }

      mat.normalTexture = normalTexture;
    }

    // Has a diffuse texture
    if (pbrInfo.baseColorTexture.has_value()) {
      auto& baseColorTextureInfo = pbrInfo.baseColorTexture.value();

      auto baseColorTextureResult = getTexture(baseColorTextureInfo.textureIndex);
      if (!baseColorTextureResult.has_value()) {
        return std::unexpected{baseColorTextureResult.error()};
      }

      auto baseColorTexture = baseColorTextureResult.value();

      if (baseColorTextureInfo.transform) {
        auto& transform = *baseColorTextureInfo.transform;

        baseColorTexture.uvOffset = glm::vec2(transform.uvOffset[0], transform.uvOffset[1]);
        baseColorTexture.uvScale *= glm::vec2(transform.uvScale[0], transform.uvScale[1]);
        baseColorTexture.uvRotation = transform.rotation;
      }

      mat.diffuseTexture = baseColorTexture;
    }

    // Has an emissive texture
    if (gltfMaterial.emissiveTexture.has_value()) {
      auto& emissiveTextureInfo = gltfMaterial.emissiveTexture.value();

      auto emissiveTextureResult = getTexture(emissiveTextureInfo.textureIndex);
      if (!emissiveTextureResult.has_value()) {
        return std::unexpected{emissiveTextureResult.error()};
      }

      auto emissiveTexture = emissiveTextureResult.value();

      if (emissiveTextureInfo.transform) {
        auto& transform = *emissiveTextureInfo.transform;

        emissiveTexture.uvOffset = glm::vec2(transform.uvOffset[0], transform.uvOffset[1]);
        emissiveTexture.uvScale *= glm::vec2(transform.uvScale[0], transform.uvScale[1]);
        emissiveTexture.uvRotation = transform.rotation;
      }

      mat.emissiveTexture = emissiveTexture;
    }

    mat.isDoubleSided = gltfMaterial.doubleSided;

    // TODO: This works for phong materials,
    // since we can just remove diffuse and specular.
    // But might not work for PBR later on.
    //
    // Also, not super intuitive
    if (gltfMaterial.unlit) {
      mat.ambient = glm::vec3(1.0f);
      mat.diffuse = glm::vec3(0.0f);
      mat.specular = glm::vec3(0.0f);
      mat.shininess = 0.0f;
    }

    materials.push_back(mat);
  }

  return std::move(materials);
};
