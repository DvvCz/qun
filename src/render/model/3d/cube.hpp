#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <array>

#include "render/model/model.hpp"
#include "render/vertex.hpp"

namespace model {
  class Cube : public Model3D {
  public:
    Cube(glm::vec3 scale);
    ~Cube();
    void draw() const;

  private:
    glm::vec3 scale;

    std::array<Vertex3D, 24> vertices; // 4 vertices per face, 6 faces
    std::array<GLuint, 36> indices;    // 2 triangles per face, 3 indices per triangle, 6 faces
    GLuint glAttributesIdx;
    GLuint glBufferIdx;
    GLuint glIndexBufferIdx;
  };
}
