#include "sphere.hpp"

#include <cmath>

#include "constants.hpp"

model::Sphere::Sphere(float radius, int rings, int sectors) : radius(radius), rings(rings), sectors(sectors) {
  glCreateVertexArrays(1, &glAttributesIdx);
  glCreateBuffers(1, &glBufferIdx);
  glCreateBuffers(1, &glIndexBufferIdx);

  {
    GLuint glAttrSlot1 = 0;

    glVertexArrayVertexBuffer(glAttributesIdx, glAttrSlot1, glBufferIdx, 0, sizeof(Vertex3D));

    glVertexArrayAttribFormat(glAttributesIdx, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(glAttributesIdx, 0);
    glVertexArrayAttribBinding(glAttributesIdx, 0, glAttrSlot1);

    glVertexArrayAttribFormat(glAttributesIdx, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, normal));
    glEnableVertexArrayAttrib(glAttributesIdx, 1);
    glVertexArrayAttribBinding(glAttributesIdx, 1, glAttrSlot1);

    glVertexArrayAttribFormat(glAttributesIdx, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, uv));
    glEnableVertexArrayAttrib(glAttributesIdx, 2);
    glVertexArrayAttribBinding(glAttributesIdx, 2, glAttrSlot1);

    glVertexArrayAttribFormat(glAttributesIdx, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, tangent));
    glEnableVertexArrayAttrib(glAttributesIdx, 3);
    glVertexArrayAttribBinding(glAttributesIdx, 3, glAttrSlot1);

    glVertexArrayElementBuffer(glAttributesIdx, glIndexBufferIdx);
  }

  generateUVSphere(radius, rings, sectors);

  glNamedBufferData(glBufferIdx, vertices.size() * sizeof(Vertex3D), vertices.data(), GL_STATIC_DRAW);
  glNamedBufferData(glIndexBufferIdx, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

model::Sphere::~Sphere() {
  glDeleteVertexArrays(1, &glAttributesIdx);
  glDeleteBuffers(1, &glBufferIdx);
  glDeleteBuffers(1, &glIndexBufferIdx);
}

void model::Sphere::draw() const {
  glBindVertexArray(glAttributesIdx);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void model::Sphere::generateUVSphere(float radius, int rings, int sectors) {
  vertices.clear();
  indices.clear();

  // Generate vertices
  for (int r = 0; r <= rings; ++r) {
    float v = static_cast<float>(r) / static_cast<float>(rings);
    float phi = v * constants::PI; // Latitude angle from 0 to PI

    for (int s = 0; s <= sectors; ++s) {
      float u = static_cast<float>(s) / static_cast<float>(sectors);
      float theta = u * constants::TAU; // Longitude angle from 0 to 2*PI

      // Calculate position using spherical coordinates
      float x = std::sin(phi) * std::cos(theta);
      float y = std::cos(phi);
      float z = std::sin(phi) * std::sin(theta);

      glm::vec3 position = glm::vec3(x, y, z) * radius;
      glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
      glm::vec2 uv = glm::vec2(u, v);

      // Calculate tangent for normal mapping
      glm::vec3 tangent = glm::normalize(glm::vec3(-std::sin(theta), 0.0f, std::cos(theta)));

      vertices.push_back({position, normal, uv, tangent});
    }
  }

  // Generate indices for triangles
  for (int r = 0; r < rings; ++r) {
    for (int s = 0; s < sectors; ++s) {
      int current = r * (sectors + 1) + s;
      int next = current + sectors + 1;

      // First triangle
      indices.push_back(current);
      indices.push_back(next);
      indices.push_back(current + 1);

      // Second triangle
      indices.push_back(current + 1);
      indices.push_back(next);
      indices.push_back(next + 1);
    }
  }
}

glm::vec2 model::Sphere::calculateSphericalUV(const glm::vec3& normal) {
  float u = 0.5f + std::atan2(normal.z, normal.x) / constants::TAU;
  float v = 0.5f - std::asin(normal.y) / constants::PI;
  return glm::vec2(u, v);
}
