#pragma once

#include <glad/glad.h>
#include <array>

#include "model.hpp"
#include "../vertex.hpp"

class QuadModel : public Model {
public:
  QuadModel(Vertex v1, Vertex v2, Vertex v3, Vertex v4);
  ~QuadModel();
  void draw() const;

private:
  std::array<Vertex, 4> vertices;
  GLuint glAttributesIdx;
  GLuint glBufferIdx;
};
