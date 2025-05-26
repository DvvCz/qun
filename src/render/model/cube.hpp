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

  std::array<Vertex, 8> vertices;
  GLuint glAttributesIdx;
  GLuint glBufferIdx;
};
