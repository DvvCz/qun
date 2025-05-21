#include "model.hpp"
#include <print>

TriangleModel::TriangleModel(Vertex v1, Vertex v2, Vertex v3) {
  glCreateVertexArrays(1, &glAttributesIdx);
  glCreateBuffers(1, &glBufferIdx);

  {
    glVertexArrayAttribFormat(glAttributesIdx, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
    glEnableVertexArrayAttrib(glAttributesIdx, 0);

    glVertexArrayAttribFormat(glAttributesIdx, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glEnableVertexArrayAttrib(glAttributesIdx, 1);

    glVertexArrayVertexBuffer(glAttributesIdx, 0, glBufferIdx, 0, sizeof(Vertex));
  }

  vertices = {v1, v2, v3};
  glNamedBufferData(glBufferIdx, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
}

TriangleModel::~TriangleModel() {
  glDeleteVertexArrays(1, &glAttributesIdx);
  glDeleteBuffers(1, &glBufferIdx);
}

void TriangleModel::draw() const {
  glBindVertexArray(glAttributesIdx);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}

AssetModel::AssetModel(const resource::ObjAsset& asset) : indices(asset.getIndices()) {
  glCreateVertexArrays(1, &glAttributesIdx);
  glCreateBuffers(1, &glBufferIdx);
  glCreateBuffers(1, &glIndexBufferIdx);

  {
    glVertexArrayAttribFormat(glAttributesIdx, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
    glEnableVertexArrayAttrib(glAttributesIdx, 0);

    glVertexArrayAttribFormat(glAttributesIdx, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glEnableVertexArrayAttrib(glAttributesIdx, 1);

    glVertexArrayVertexBuffer(glAttributesIdx, 0, glBufferIdx, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(glAttributesIdx, glIndexBufferIdx); // Bind index buffer to VAO
  }

  // Convert vertices from std::array<float, 3> to Vertex struct
  std::vector<Vertex> vertices;
  auto rawVertices = asset.getVertices();
  vertices.reserve(rawVertices.size());

  for (const auto& rawVertex : rawVertices) {
    Vertex vertex;
    vertex.pos = glm::vec3(rawVertex[0], rawVertex[1], rawVertex[2]);
    // For simplicity, using position as normal until normal data is available
    vertex.normal = glm::normalize(glm::vec3(rawVertex[0], rawVertex[1], rawVertex[2]));
    vertices.push_back(vertex);
  }

  glNamedBufferData(glBufferIdx, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

  // Convert signed indices to unsigned indices for OpenGL
  std::vector<GLuint> unsignedIndices(indices.begin(), indices.end());
  glNamedBufferData(glIndexBufferIdx, sizeof(GLuint) * unsignedIndices.size(), unsignedIndices.data(), GL_STATIC_DRAW);
}

AssetModel::~AssetModel() {
  glDeleteVertexArrays(1, &glAttributesIdx);
  glDeleteBuffers(1, &glBufferIdx);
  glDeleteBuffers(1, &glIndexBufferIdx);
}

void AssetModel::draw() const {
  glBindVertexArray(glAttributesIdx);
  glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}