#pragma once

#include "render/model/model.hpp"
#include "render/material/material3d.hpp"
#include "render/texture.hpp"

#include "asset/obj/obj.hpp"

namespace model {
  class Asset : public Model3D {
  public:
    Asset(const asset::Obj& asset, std::shared_ptr<TextureManager> texMan, std::shared_ptr<material::Manager3D> matMan);
    ~Asset();
    void draw() const;

  private:
    std::shared_ptr<TextureManager> textureManager;
    std::shared_ptr<material::Manager3D> materialManager;

    std::vector<asset::ObjMaterialGroup> materialGroups;

    asset::Obj inner;
    std::vector<GLuint> allIndices;
    GLuint glAttributesIdx;
    GLuint glBufferIdx;
    GLuint glIndexBufferIdx;
  };
};
