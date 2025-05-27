#pragma once

#include <rapidobj/rapidobj.hpp>

#include "render/uniform.hpp"
#include "render/texture.hpp"

namespace material {
  struct Block2D {
    alignas(16) glm::vec3 color;
  };

  class Manager2D {
  public:
    Manager2D(UniformBlock<Block2D> uniformMaterialBlock, std::shared_ptr<texture::Manager> texMan);
    ~Manager2D();

    void setMaterial(const Block2D& material) noexcept;
    [[nodiscard]] Block2D getMaterial() const noexcept;

  private:
    std::shared_ptr<texture::Manager> textureManager;

    UniformBlock<Block2D> uniformMaterialBlock;
    Block2D currentMaterial;
  };
};
