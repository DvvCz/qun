#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <array>

#include "render/model/model.hpp"
#include "render/vertex.hpp"

namespace model {
  class Quad : public Model2D {
  public:
    Quad(Vertex2D q1, Vertex2D q2, Vertex2D q3, Vertex2D q4);
    ~Quad();
    void draw() const;

  private:
    std::array<Vertex2D, 4> vertices;
    GLuint glAttributesIdx;
    GLuint glBufferIdx;
  };
}
