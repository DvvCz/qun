#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>

#include "../resource/asset.hpp"
#include "../resource/obj/obj.hpp"
#include "vertex.hpp"

class Model {
public:
  virtual void draw() const = 0;
};

class TriangleModel : public Model {
public:
  TriangleModel(Vertex v1, Vertex v2, Vertex v3);
  ~TriangleModel();
  void draw() const;

private:
  std::array<Vertex, 3> vertices;
  GLuint glAttributesIdx;
  GLuint glBufferIdx;
};

class AssetModel : public Model {
public:
  AssetModel(const resource::ObjAsset& asset);
  ~AssetModel();
  void draw() const;

private:
  std::vector<int> indices;
  GLuint glAttributesIdx;
  GLuint glBufferIdx;
  GLuint glIndexBufferIdx;
};