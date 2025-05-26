#pragma once

#include "model.hpp"
#include "../material.hpp"
#include "../texture.hpp"
#include "../../resource/obj/obj.hpp"

class AssetModel : public Model {
public:
  AssetModel(const resource::ObjAsset& asset, std::shared_ptr<TextureManager> texMan, std::shared_ptr<MaterialManager> matMan);
  ~AssetModel();
  void draw() const;

private:
  std::shared_ptr<TextureManager> textureManager;
  std::shared_ptr<MaterialManager> materialManager;

  std::vector<resource::ObjMaterialGroup> materialGroups;

  resource::ObjAsset inner;
  std::vector<GLuint> allIndices;
  GLuint glAttributesIdx;
  GLuint glBufferIdx;
  GLuint glIndexBufferIdx;
};
