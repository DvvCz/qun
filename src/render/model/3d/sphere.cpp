#include "sphere.hpp"
#include <array>
#include <cmath>
#include <unordered_map>

namespace {
  // Hash function for glm::vec3 to use in unordered_map
  struct Vec3Hash {
    std::size_t operator()(const glm::vec3& v) const {
      return std::hash<float>()(v.x) ^ (std::hash<float>()(v.y) << 1) ^ (std::hash<float>()(v.z) << 2);
    }
  };

  // Equality function for glm::vec3
  struct Vec3Equal {
    bool operator()(const glm::vec3& a, const glm::vec3& b) const {
      const float epsilon = 1e-6f;
      return std::abs(a.x - b.x) < epsilon && std::abs(a.y - b.y) < epsilon && std::abs(a.z - b.z) < epsilon;
    }
  };
}

model::Sphere::Sphere(float radius, int subdivisions) : radius(radius), subdivisions(subdivisions) {
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

  generateIcosphere(radius, subdivisions);

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

void model::Sphere::generateIcosphere(float radius, int subdivisions) {
  vertices.clear();
  indices.clear();

  // Create icosahedron vertices
  const float t = (1.0f + std::sqrt(5.0f)) / 2.0f; // Golden ratio

  std::vector<glm::vec3> icosahedronVertices = {
      glm::normalize(glm::vec3(-1, t, 0)),  glm::normalize(glm::vec3(1, t, 0)),   glm::normalize(glm::vec3(-1, -t, 0)),
      glm::normalize(glm::vec3(1, -t, 0)),  glm::normalize(glm::vec3(0, -1, t)),  glm::normalize(glm::vec3(0, 1, t)),
      glm::normalize(glm::vec3(0, -1, -t)), glm::normalize(glm::vec3(0, 1, -t)),  glm::normalize(glm::vec3(t, 0, -1)),
      glm::normalize(glm::vec3(t, 0, 1)),   glm::normalize(glm::vec3(-t, 0, -1)), glm::normalize(glm::vec3(-t, 0, 1))};

  // Create icosahedron faces
  std::vector<std::array<int, 3>> icosahedronFaces = {
      {0, 11, 5}, {0, 5, 1}, {0, 1, 7}, {0, 7, 10}, {0, 10, 11}, {1, 5, 9}, {5, 11, 4}, {11, 10, 2}, {10, 7, 6}, {7, 1, 8},
      {3, 9, 4},  {3, 4, 2}, {3, 2, 6}, {3, 6, 8},  {3, 8, 9},   {4, 9, 5}, {2, 4, 11}, {6, 2, 10},  {8, 6, 7},  {9, 8, 1}};

  // Use a map to avoid duplicate vertices
  std::unordered_map<glm::vec3, GLuint, Vec3Hash, Vec3Equal> vertexMap;

  auto addVertex = [&](const glm::vec3& position) -> GLuint {
    auto it = vertexMap.find(position);
    if (it != vertexMap.end()) {
      return it->second;
    }

    GLuint index = vertices.size();
    glm::vec3 normal = glm::normalize(position);
    glm::vec3 scaledPosition = normal * radius;
    glm::vec2 uv = calculateSphericalUV(normal);

    vertices.push_back({scaledPosition, normal, uv});
    vertexMap[position] = index;
    return index;
  };

  // Add initial icosahedron vertices
  for (const auto& vertex : icosahedronVertices) {
    addVertex(vertex);
  }

  // Add initial faces
  std::vector<std::array<GLuint, 3>> faces;
  for (const auto& face : icosahedronFaces) {
    faces.push_back({static_cast<GLuint>(face[0]), static_cast<GLuint>(face[1]), static_cast<GLuint>(face[2])});
  }

  // Subdivide faces
  for (int level = 0; level < subdivisions; ++level) {
    std::vector<std::array<GLuint, 3>> newFaces;

    for (const auto& face : faces) {
      glm::vec3 v1 = glm::normalize(vertices[face[0]].pos / radius);
      glm::vec3 v2 = glm::normalize(vertices[face[1]].pos / radius);
      glm::vec3 v3 = glm::normalize(vertices[face[2]].pos / radius);

      // Calculate midpoints and normalize them to the sphere surface
      glm::vec3 m1 = glm::normalize((v1 + v2) * 0.5f);
      glm::vec3 m2 = glm::normalize((v2 + v3) * 0.5f);
      glm::vec3 m3 = glm::normalize((v3 + v1) * 0.5f);

      GLuint i1 = addVertex(v1);
      GLuint i2 = addVertex(v2);
      GLuint i3 = addVertex(v3);
      GLuint im1 = addVertex(m1);
      GLuint im2 = addVertex(m2);
      GLuint im3 = addVertex(m3);

      // Create 4 new triangles
      newFaces.push_back({i1, im1, im3});
      newFaces.push_back({i2, im2, im1});
      newFaces.push_back({i3, im3, im2});
      newFaces.push_back({im1, im2, im3});
    }

    faces = std::move(newFaces);
  }

  // Convert faces to indices
  for (const auto& face : faces) {
    indices.push_back(face[0]);
    indices.push_back(face[1]);
    indices.push_back(face[2]);
  }
}

glm::vec2 model::Sphere::calculateSphericalUV(const glm::vec3& normal) {
  float u = 0.5f + std::atan2(normal.z, normal.x) / (2.0f * M_PI);
  float v = 0.5f - std::asin(normal.y) / M_PI;
  return glm::vec2(u, v);
}
