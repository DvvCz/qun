#pragma once

#include <glad/glad.h>
#include <array>

#include "model.hpp"
#include "../vertex.hpp"

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
