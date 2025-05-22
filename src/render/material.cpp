#include "material.hpp"

MaterialManager::MaterialManager(UniformBlock<MaterialBlock> uniformMaterialBlock)
    : uniformMaterialBlock(uniformMaterialBlock) {
}

MaterialManager::~MaterialManager() {
}

void MaterialManager::setMaterial(const resource::ObjMaterial& material) noexcept {
  MaterialBlock newMaterial = {/* clang-format off */
    .ambient = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]),
    .diffuse = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]),
    .specular = glm::vec3(material.specular[0], material.specular[1], material.specular[2]),
    .shininess = material.shininess,
    .dissolve = material.dissolve
  };/* clang-format on */

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
