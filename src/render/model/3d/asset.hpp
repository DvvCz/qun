#pragma once

#include "render/model/model.hpp"
#include "render/material/material3d.hpp"
#include "render/texture.hpp"

#include "resource/obj/obj.hpp"

namespace model {
  class Asset : public Model3D {
  public:
    Asset(const resource::ObjAsset& asset, std::shared_ptr<TextureManager> texMan, std::shared_ptr<material::Manager3D> matMan);
    ~Asset();
    void draw() const;

  private:
    std::shared_ptr<TextureManager> textureManager;
    std::shared_ptr<material::Manager3D> materialManager;

    std::vector<resource::ObjMaterialGroup> materialGroups;

    resource::ObjAsset inner;
    std::vector<GLuint> allIndices;
    GLuint glAttributesIdx;
    GLuint glBufferIdx;
    GLuint glIndexBufferIdx;
  };
};
