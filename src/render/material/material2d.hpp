#pragma once

#include <rapidobj/rapidobj.hpp>

#include "render/uniform/block.hpp"
#include "render/texture.hpp"

namespace material {
  struct Material2D {
    alignas(16) glm::vec3 color;
  };

  class Manager2D {
  public:
    Manager2D(uniform::Block<Material2D> uniformMaterial, std::shared_ptr<texture::Manager> texMan);
    ~Manager2D();

    void setMaterial(const Material2D& material) noexcept;
    [[nodiscard]] Material2D getMaterial() const noexcept;

  private:
    std::shared_ptr<texture::Manager> textureManager;

    uniform::Block<Material2D> uniformMaterial;
    Material2D currentMaterial;
  };
};
