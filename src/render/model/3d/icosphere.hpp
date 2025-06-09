#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "render/model/model.hpp"
#include "render/vertex.hpp"

namespace model {
  class Icosphere : public Model3D {
  public:
    Icosphere(float radius = 1.0f, int subdivisions = 2);
    ~Icosphere();
    void draw() const;

  private:
    void generateIcosphere(float radius, int subdivisions);
    void subdivideTriangle(const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3, int depth);
    glm::vec2 calculateSphericalUV(const glm::vec3& normal);

    float radius;
    int subdivisions;

    std::vector<Vertex3D> vertices;
    std::vector<GLuint> indices;
    GLuint glAttributesIdx;
    GLuint glBufferIdx;
    GLuint glIndexBufferIdx;
  };
}
