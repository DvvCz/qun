#include "material2d.hpp"
#include "render/texture.hpp"

material::Manager2D::Manager2D(uniform::Block<material::Material2D> uniformMaterial, std::shared_ptr<texture::Manager> texMan)
    : uniformMaterial(uniformMaterial), textureManager(texMan) {
}

material::Manager2D::~Manager2D() {
}

void material::Manager2D::setMaterial(const material::Material2D& material) noexcept {
  uniformMaterial.set(material);
  currentMaterial = material;
}

material::Material2D material::Manager2D::getMaterial() const noexcept {
  return currentMaterial;
}
