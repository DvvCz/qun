#include "material.hpp"
#include "texture.hpp"

#include <print>

MaterialManager::MaterialManager(UniformBlock<MaterialBlock> uniformMaterialBlock, std::shared_ptr<TextureManager> texMan)
    : uniformMaterialBlock(uniformMaterialBlock), textureManager(texMan) {
}

MaterialManager::~MaterialManager() {
}

void MaterialManager::setMaterial(const resource::ObjMaterial& material) noexcept {
  /* clang-format off */
  MaterialBlock newMaterial = {
    .ambient = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]),
    .diffuse = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]),
    .specular = glm::vec3(material.specular[0], material.specular[1], material.specular[2]),
    .shininess = material.shininess,
    .dissolve = material.dissolve,
  };/* clang-format on */

  if (material.diffuseTexture.has_value()) {
    // textureManager->bindTexture(0);
    auto out = textureManager->getTextureByPath(material.diffuseTexture.value());
    if (out.has_value()) {
      textureManager->bindTexture(out.value());
    } else {
      std::println(stderr, "Failed to get texture by path: {}", material.diffuseTexture.value());
      textureManager->unbindTexture();
    }
  } else {
    textureManager->unbindTexture();
  }

  uniformMaterialBlock.set(newMaterial);
  currentMaterial = newMaterial;
}

void MaterialManager::setMaterial(const MaterialBlock material) noexcept {
  uniformMaterialBlock.set(material);
  currentMaterial = material;
}

MaterialBlock MaterialManager::getMaterial() const noexcept {
  return currentMaterial;
}
