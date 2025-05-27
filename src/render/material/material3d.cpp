#include "material3d.hpp"

#include "render/texture.hpp"

#include <print>

material::Manager3D::Manager3D(UniformBlock<material::Block3D> uniformMaterialBlock, std::shared_ptr<TextureManager> texMan)
    : uniformMaterialBlock(uniformMaterialBlock), textureManager(texMan) {
}

material::Manager3D::~Manager3D() {
}

void material::Manager3D::setMaterial(const asset::Material& material) noexcept {
  /* clang-format off */
  material::Block3D newMaterial = {
    .ambient = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]),
    .diffuse = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]),
    .specular = glm::vec3(material.specular[0], material.specular[1], material.specular[2]),
    .shininess = material.shininess,
    .dissolve = material.dissolve,
    .diffuseTextureId = -1
  };/* clang-format on */

  if (material.diffuseTexture.has_value()) {
    auto out = textureManager->getTextureByPath(material.diffuseTexture.value());
    if (out.has_value()) {
      newMaterial.diffuseTextureId = out.value();
    } else {
      std::println(stderr, "Failed to get texture by path: {}", material.diffuseTexture.value().generic_string());
    }
  }

  uniformMaterialBlock.set(newMaterial);
  currentMaterial = newMaterial;
}

void material::Manager3D::setMaterial(const material::Block3D& material) noexcept {
  uniformMaterialBlock.set(material);
  currentMaterial = material;
}

material::Block3D material::Manager3D::getMaterial() const noexcept {
  return currentMaterial;
}
