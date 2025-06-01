#pragma once

#include <rapidobj/rapidobj.hpp>

#include "render/uniform/block.hpp"
#include "render/texture.hpp"

namespace material {
  struct Material3D {
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
    float shininess;
    float dissolve;

    texture::Texture diffuseTexture;
    texture::Texture normalTexture;
  };

  class Manager3D {
  public:
    Manager3D(uniform::Block<material::Material3D> uniformMaterial, std::shared_ptr<texture::Manager> texMan);
    ~Manager3D();

    void setMaterial(const asset::Material& material) noexcept;
    void setMaterial(const material::Material3D& material) noexcept;
    [[nodiscard]] material::Material3D getMaterial() const noexcept;

  private:
    std::shared_ptr<texture::Manager> textureManager;

    uniform::Block<material::Material3D> uniformMaterial;
    material::Material3D currentMaterial;
  };
};
