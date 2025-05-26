#include "cube.hpp"

CubeModel::CubeModel(glm::vec3 pos, glm::vec3 size, glm::quat rot) : pos(pos), size(size), rot(rot) {
  glCreateVertexArrays(1, &glAttributesIdx);
  glCreateBuffers(1, &glBufferIdx);

  {
    glVertexArrayAttribFormat(glAttributesIdx, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
    glEnableVertexArrayAttrib(glAttributesIdx, 0);

    glVertexArrayAttribFormat(glAttributesIdx, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glEnableVertexArrayAttrib(glAttributesIdx, 1);

    glVertexArrayAttribFormat(glAttributesIdx, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
    glEnableVertexArrayAttrib(glAttributesIdx, 2);

    glVertexArrayVertexBuffer(glAttributesIdx, 0, glBufferIdx, 0, sizeof(Vertex));
  }

  // Define the vertices for a cube
  vertices = {
      Vertex{pos, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},                                    // Front top left
      Vertex{pos + glm::vec3(size.x, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},    // Front top right
      Vertex{pos + glm::vec3(size.x, size.y, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},  // Front bottom right
      Vertex{pos + glm::vec3(0.0f, size.y, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},    // Front bottom left
      Vertex{pos + glm::vec3(0.0f, 0.0f, size.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},   // Back top left
      Vertex{pos + glm::vec3(size.x, 0.0f, size.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)}, // Back top right
      Vertex{pos + glm::vec3(size.x, size.y, size.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)}, // Back bottom right
      Vertex{pos + glm::vec3(0.0f, size.y, size.z), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)}    // Back bottom left
  };

  // Apply transformations
  for (auto& vertex : vertices) {
    vertex.pos = rot * (vertex.pos * size) + pos;
  }

  glNamedBufferData(glBufferIdx, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
}

CubeModel::~CubeModel() {
  glDeleteVertexArrays(1, &glAttributesIdx);
  glDeleteBuffers(1, &glBufferIdx);
}

void CubeModel::draw() const {
  glBindVertexArray(glAttributesIdx);

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Front face
  glDrawArrays(GL_TRIANGLE_FAN, 4, 4); // Back face

  // Draw sides
  for (int i = 0; i < 4; ++i) {
    glDrawArrays(GL_TRIANGLE_STRIP, i, 4);
  }
}
