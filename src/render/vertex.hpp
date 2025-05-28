#pragma once

#include <glm/glm.hpp>

struct Vertex3D {
  glm::vec3 pos;
  glm::vec3 normal;
  glm::vec2 uv;
  glm::vec3 tangent;
};

struct Vertex2D {
  glm::vec2 pos;
  glm::vec2 uv;
};
