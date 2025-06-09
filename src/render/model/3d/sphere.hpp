#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "render/model/model.hpp"
#include "render/vertex.hpp"

namespace model {
  class Sphere : public Model3D {
  public:
    Sphere(float radius = 1.0f, int rings = 16, int sectors = 32);
    ~Sphere();
    void draw() const;

  private:
    void generateUVSphere(float radius, int rings, int sectors);
    glm::vec2 calculateSphericalUV(const glm::vec3& normal);

    float radius;
    int rings;
    int sectors;

    std::vector<Vertex3D> vertices;
    std::vector<GLuint> indices;
    GLuint glAttributesIdx;
    GLuint glBufferIdx;
    GLuint glIndexBufferIdx;
  };
}
