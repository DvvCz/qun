#include "material3d.hpp"

#include "render/texture.hpp"
#include <print>

material::Manager3D::Manager3D(uniform::Block<material::Material3D> uniformMaterial, std::shared_ptr<texture::Manager> texMan)
    : uniformMaterial(uniformMaterial), textureManager(texMan) {
}

material::Manager3D::~Manager3D() {
}

// todo: might be able to combine these in the future.
void material::Manager3D::setMaterial(const asset::Material& material) noexcept {
  texture::Texture diffuseTexture;
  if (material.diffuseTexture.has_value()) {
    diffuseTexture.index = material.diffuseTexture->index;
    diffuseTexture.uvScale = material.diffuseTexture->uvScale;
    diffuseTexture.uvOffset = material.diffuseTexture->uvOffset;
    diffuseTexture.uvRotation = material.diffuseTexture->uvRotation;
  }

  texture::Texture normalTexture;
  if (material.normalTexture.has_value()) {
    normalTexture.index = material.normalTexture->index;
    normalTexture.uvScale = material.normalTexture->uvScale;
    normalTexture.uvOffset = material.normalTexture->uvOffset;
    normalTexture.uvRotation = material.normalTexture->uvRotation;
  }

  /* clang-format off */
  material::Material3D newMaterial = {
    .ambient = material.ambient,
    .shininess = material.shininess,
    .diffuse = material.diffuse,
    .dissolve = material.dissolve,
    .specular = material.specular,
    .diffuseTexture = diffuseTexture,
    .normalTexture = normalTexture
  };/* clang-format on */

  uniformMaterial.set(newMaterial);
  currentMaterial = newMaterial;
}

void material::Manager3D::setMaterial(const material::Material3D& material) noexcept {
  uniformMaterial.set(material);
  currentMaterial = material;
}

material::Material3D material::Manager3D::getMaterial() const noexcept {
  return currentMaterial;
}
