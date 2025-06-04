#include "material3d.hpp"

#include "render/texture.hpp"
#include <glm/matrix.hpp>
#include <print>

material::Manager3D::Manager3D(uniform::Block<material::Material3D> uniformMaterial, std::shared_ptr<texture::Manager> texMan)
    : uniformMaterial(uniformMaterial), textureManager(texMan) {
}

material::Manager3D::~Manager3D() {
}

// todo: might be able to combine these in the future.
void material::Manager3D::setMaterial(const asset::Material& material) noexcept {
  /* clang-format off */
  material::Material3D newMaterial = {
    .ambient = material.ambient,
    .shininess = material.shininess,
    .diffuse = material.diffuse,
    .dissolve = material.dissolve,
    .specular = material.specular,
    .diffuseTexture = material.diffuseTexture.value_or(texture::Texture{}),
    .normalTexture = material.normalTexture.value_or(texture::Texture{}),
    .emissiveTexture = material.emissiveTexture.value_or(texture::Texture{}),
  };/* clang-format on */

  setMaterial(newMaterial);
}

void material::Manager3D::setMaterial(const material::Material3D& material) noexcept {
  uniformMaterial.set(material);
  currentMaterial = material;
}

material::Material3D material::Manager3D::getMaterial() const noexcept {
  return currentMaterial;
}
