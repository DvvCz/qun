#pragma once

#include <rapidobj/rapidobj.hpp>

#include "render/uniform/block.hpp"
#include "render/texture.hpp"

namespace material {
  struct Block3D {
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
    float shininess;
    float dissolve;

    // -1 for no texture
    int diffuseTextureId = -1;
    int normalTextureId = -1;
  };

  class Manager3D {
  public:
    Manager3D(uniform::Block<Block3D> uniformMaterialBlock, std::shared_ptr<texture::Manager> texMan);
    ~Manager3D();

    void setMaterial(const asset::Material& material) noexcept;
    void setMaterial(const Block3D& material) noexcept;
    [[nodiscard]] Block3D getMaterial() const noexcept;

  private:
    std::shared_ptr<texture::Manager> textureManager;

    uniform::Block<Block3D> uniformMaterialBlock;
    Block3D currentMaterial;
  };
};
