#include "material2d.hpp"
#include "render/texture.hpp"

material::Manager2D::Manager2D(UniformBlock<material::Block2D> uniformMaterialBlock, std::shared_ptr<texture::Manager> texMan)
    : uniformMaterialBlock(uniformMaterialBlock), textureManager(texMan) {
}

material::Manager2D::~Manager2D() {
}

void material::Manager2D::setMaterial(const material::Block2D& material) noexcept {
  uniformMaterialBlock.set(material);
  currentMaterial = material;
}

material::Block2D material::Manager2D::getMaterial() const noexcept {
  return currentMaterial;
}
