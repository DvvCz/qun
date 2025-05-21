#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>

struct Vertex {
  glm::vec3 pos;
  glm::vec3 normal;
};

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