#include "material3d.hpp"

#include "render/texture.hpp"
#include <print>

material::Manager3D::Manager3D(uniform::Block<material::Material3D> uniformMaterial, std::shared_ptr<texture::Manager> texMan)
    : uniformMaterial(uniformMaterial), textureManager(texMan) {
}

material::Manager3D::~Manager3D() {
}

static texture::Texture assetTexToGLTex(const std::optional<asset::Texture> tex) noexcept {
  texture::Texture out;

  if (tex.has_value()) {
    out.index = tex->index;
    out.uvScale = tex->uvScale;
    out.uvOffset = tex->uvOffset;
    out.uvRotation = tex->uvRotation;
  }

  return out;
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
    .diffuseTexture = assetTexToGLTex(material.diffuseTexture),
    .normalTexture = assetTexToGLTex(material.normalTexture),
    .emissiveTexture = assetTexToGLTex(material.emissiveTexture),
  };/* clang-format on */

  if (material.isDoubleSided) {
    glDisable(GL_CULL_FACE);
  } else {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  }

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
