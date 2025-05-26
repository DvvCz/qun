#include "cube.hpp"

CubeModel::CubeModel(glm::vec3 pos, glm::vec3 size, glm::quat rot) : pos(pos), size(size), rot(rot) {
  glCreateVertexArrays(1, &glAttributesIdx);
  glCreateBuffers(1, &glBufferIdx);
  glCreateBuffers(1, &glIndexBufferIdx);

  {
    GLuint glAttrSlot1 = 0;

    glVertexArrayAttribFormat(glAttributesIdx, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(glAttributesIdx, 0);
    glVertexArrayAttribBinding(glAttributesIdx, 0, glAttrSlot1);

    glVertexArrayAttribFormat(glAttributesIdx, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal) - offsetof(Vertex, pos));
    glEnableVertexArrayAttrib(glAttributesIdx, 1);
    glVertexArrayAttribBinding(glAttributesIdx, 1, glAttrSlot1);

    glVertexArrayAttribFormat(glAttributesIdx, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv) - offsetof(Vertex, normal));
    glEnableVertexArrayAttrib(glAttributesIdx, 2);
    glVertexArrayAttribBinding(glAttributesIdx, 2, glAttrSlot1);

    glVertexArrayVertexBuffer(glAttributesIdx, glAttrSlot1, glBufferIdx, 0, sizeof(Vertex));
    glVertexArrayElementBuffer(glAttributesIdx, glIndexBufferIdx);
  }

  // Each face has its own vertices to allow for proper normals and UVs
  vertices = {/* clang-format off */
      // Front face (Y+)
      Vertex{glm::vec3(-.5f,  .5f, -.5f), glm::vec3(0, 1, 0), glm::vec2(0, 0)},
      Vertex{glm::vec3( .5f,  .5f, -.5f), glm::vec3(0, 1, 0), glm::vec2(1, 0)},
      Vertex{glm::vec3( .5f,  .5f,  .5f), glm::vec3(0, 1, 0), glm::vec2(1, 1)},
      Vertex{glm::vec3(-.5f,  .5f,  .5f), glm::vec3(0, 1, 0), glm::vec2(0, 1)},

      // Back face (Y-)
      Vertex{glm::vec3( .5f, -.5f, -.5f), glm::vec3(0, -1, 0), glm::vec2(0, 0)},
      Vertex{glm::vec3(-.5f, -.5f, -.5f), glm::vec3(0, -1, 0), glm::vec2(1, 0)},
      Vertex{glm::vec3(-.5f, -.5f,  .5f), glm::vec3(0, -1, 0), glm::vec2(1, 1)},
      Vertex{glm::vec3( .5f, -.5f,  .5f), glm::vec3(0, -1, 0), glm::vec2(0, 1)},

      // Left face (X-)
      Vertex{glm::vec3(-.5f, -.5f, -.5f), glm::vec3(-1, 0, 0), glm::vec2(0, 0)},
      Vertex{glm::vec3(-.5f,  .5f, -.5f), glm::vec3(-1, 0, 0), glm::vec2(1, 0)},
      Vertex{glm::vec3(-.5f,  .5f,  .5f), glm::vec3(-1, 0, 0), glm::vec2(1, 1)},
      Vertex{glm::vec3(-.5f, -.5f,  .5f), glm::vec3(-1, 0, 0), glm::vec2(0, 1)},

      // Right face (X+)
      Vertex{glm::vec3( .5f,  .5f, -.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0)},
      Vertex{glm::vec3( .5f, -.5f, -.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0)},
      Vertex{glm::vec3( .5f, -.5f,  .5f), glm::vec3(1, 0, 0), glm::vec2(1, 1)},
      Vertex{glm::vec3( .5f,  .5f,  .5f), glm::vec3(1, 0, 0), glm::vec2(0, 1)},

      // Bottom face (Z-)
      Vertex{glm::vec3(-.5f, -.5f, -.5f), glm::vec3(0, 0, -1), glm::vec2(0, 0)},
      Vertex{glm::vec3( .5f, -.5f, -.5f), glm::vec3(0, 0, -1), glm::vec2(1, 0)},
      Vertex{glm::vec3( .5f,  .5f, -.5f), glm::vec3(0, 0, -1), glm::vec2(1, 1)},
      Vertex{glm::vec3(-.5f,  .5f, -.5f), glm::vec3(0, 0, -1), glm::vec2(0, 1)},

      // Top face (Z+)
      Vertex{glm::vec3(-.5f, -.5f,  .5f), glm::vec3(0, 0, 1), glm::vec2(0, 0)},
      Vertex{glm::vec3(-.5f,  .5f,  .5f), glm::vec3(0, 0, 1), glm::vec2(1, 0)},
      Vertex{glm::vec3( .5f,  .5f,  .5f), glm::vec3(0, 0, 1), glm::vec2(1, 1)},
      Vertex{glm::vec3( .5f, -.5f,  .5f), glm::vec3(0, 0, 1), glm::vec2(0, 1)}
      }; /* clang-format on */

  indices = {/* clang-format off */
      // Front face
      0, 1, 2,   2, 3, 0,
      // Back face
      4, 5, 6,   6, 7, 4,
      // Left face
      8, 9, 10,  10, 11, 8,
      // Right face
      12, 13, 14, 14, 15, 12,
      // Bottom face
      16, 17, 18, 18, 19, 16,
      // Top face
      20, 21, 22, 22, 23, 20
  }; /* clang-format on */

  glm::mat4 transform = glm::mat4(1.0f);
  transform = glm::translate(transform, pos);
  transform = transform * glm::mat4_cast(rot);
  transform = glm::scale(transform, size);

  for (auto& vertex : vertices) {
    glm::vec4 transformedPos = transform * glm::vec4(vertex.pos, 1.0f);
    vertex.pos = glm::vec3(transformedPos);

    // Transform normals (only rotation, no translation or scale)
    glm::mat3 normalMatrix = glm::mat3(glm::mat4_cast(rot));
    vertex.normal = normalMatrix * vertex.normal;
  }

  glNamedBufferData(glBufferIdx, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
  glNamedBufferData(glIndexBufferIdx, sizeof(indices), indices.data(), GL_STATIC_DRAW);
}

CubeModel::~CubeModel() {
  glDeleteVertexArrays(1, &glAttributesIdx);
  glDeleteBuffers(1, &glBufferIdx);
  glDeleteBuffers(1, &glIndexBufferIdx);
}

void CubeModel::draw() const {
  glBindVertexArray(glAttributesIdx);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
