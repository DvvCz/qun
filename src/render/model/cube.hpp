#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <array>

#include "model.hpp"
#include "../vertex.hpp"

class CubeModel : public Model {
public:
  CubeModel(glm::vec3 pos, glm::vec3 size, glm::quat rot);
  ~CubeModel();
  void draw() const;

private:
  glm::vec3 pos;
  glm::vec3 size;
  glm::quat rot;

  std::array<Vertex, 24> vertices; // 4 vertices per face, 6 faces
  std::array<GLuint, 36> indices;  // 2 triangles per face, 3 indices per triangle, 6 faces
  GLuint glAttributesIdx;
  GLuint glBufferIdx;
  GLuint glElementBufferIdx;
};
