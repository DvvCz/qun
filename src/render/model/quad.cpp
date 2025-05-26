#include "quad.hpp"

QuadModel::QuadModel(Vertex v1, Vertex v2, Vertex v3, Vertex v4) {
  glCreateVertexArrays(1, &glAttributesIdx);
  glCreateBuffers(1, &glBufferIdx);

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
    glVertexArrayElementBuffer(glAttributesIdx, glBufferIdx);
  }

  vertices = {v1, v2, v3, v4};
  glNamedBufferData(glBufferIdx, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
}

QuadModel::~QuadModel() {
  glDeleteVertexArrays(1, &glAttributesIdx);
  glDeleteBuffers(1, &glBufferIdx);
}

void QuadModel::draw() const {
  glBindVertexArray(glAttributesIdx);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
