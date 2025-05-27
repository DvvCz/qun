#include "material3d.hpp"

#include "render/texture.hpp"

material::Manager3D::Manager3D(uniform::Block<material::Block3D> uniformMaterialBlock, std::shared_ptr<texture::Manager> texMan)
    : uniformMaterialBlock(uniformMaterialBlock), textureManager(texMan) {
}

material::Manager3D::~Manager3D() {
}

// todo: might be able to combine these in the future.
void material::Manager3D::setMaterial(const asset::Material& material) noexcept {
  /* clang-format off */
  material::Block3D newMaterial = {
    .ambient = material.ambient,
    .diffuse = material.diffuse,
    .specular = material.specular,
    .shininess = material.shininess,
    .dissolve = material.dissolve,
    .diffuseTextureId = material.diffuseTexture.has_value() ? static_cast<int>(material.diffuseTexture.value()) : -1
  };/* clang-format on */

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
