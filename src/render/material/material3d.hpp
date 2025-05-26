#pragma once

#include <rapidobj/rapidobj.hpp>

#include "render/uniform.hpp"
#include "render/texture.hpp"

#include "resource/obj/obj.hpp"

namespace material {
  struct Block3D {
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
    float shininess;
    float dissolve;
  };

  class Manager3D {
  public:
    Manager3D(UniformBlock<Block3D> uniformMaterialBlock, std::shared_ptr<TextureManager> texMan);
    ~Manager3D();

    void setMaterial(const resource::ObjMaterial& material) noexcept;
    void setMaterial(const Block3D& material) noexcept;
    [[nodiscard]] Block3D getMaterial() const noexcept;

  private:
    std::shared_ptr<TextureManager> textureManager;

    UniformBlock<Block3D> uniformMaterialBlock;
    Block3D currentMaterial;
  };
};
