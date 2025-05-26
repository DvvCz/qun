#pragma once

#include <rapidobj/rapidobj.hpp>
#include "uniform.hpp"
#include "../resource/obj/obj.hpp"
#include "texture.hpp"

struct MaterialBlock {
  alignas(16) glm::vec3 ambient;
  alignas(16) glm::vec3 diffuse;
  alignas(16) glm::vec3 specular;
  float shininess;
  float dissolve;
};

class MaterialManager {
public:
  MaterialManager(UniformBlock<MaterialBlock> uniformMaterialBlock, std::shared_ptr<TextureManager> texMan);
  ~MaterialManager();

  void setMaterial(const resource::ObjMaterial& material) noexcept;
  void setMaterial(const MaterialBlock& material) noexcept;
  [[nodiscard]] MaterialBlock getMaterial() const noexcept;

private:
  std::shared_ptr<TextureManager> textureManager;

  UniformBlock<MaterialBlock> uniformMaterialBlock;
  MaterialBlock currentMaterial;
};
