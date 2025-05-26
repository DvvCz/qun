#include "quad.hpp"

model::Quad::Quad(Vertex2D q1, Vertex2D q2, Vertex2D q3, Vertex2D q4) {
  glCreateVertexArrays(1, &glAttributesIdx);
  glCreateBuffers(1, &glBufferIdx);

  {
    GLuint glAttrSlot1 = 0;

    glVertexArrayAttribFormat(glAttributesIdx, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glEnableVertexArrayAttrib(glAttributesIdx, 0);
    glVertexArrayAttribBinding(glAttributesIdx, 0, glAttrSlot1);

    glVertexArrayAttribFormat(glAttributesIdx, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex2D, uv) - offsetof(Vertex2D, pos));
    glEnableVertexArrayAttrib(glAttributesIdx, 1);
    glVertexArrayAttribBinding(glAttributesIdx, 1, glAttrSlot1);

    glVertexArrayVertexBuffer(glAttributesIdx, glAttrSlot1, glBufferIdx, 0, sizeof(Vertex2D));
  }

  vertices = {q1, q2, q3, q4};
  glNamedBufferData(glBufferIdx, vertices.size() * sizeof(Vertex2D), vertices.data(), GL_STATIC_DRAW);
}

model::Quad::~Quad() {
  glDeleteVertexArrays(1, &glAttributesIdx);
  glDeleteBuffers(1, &glBufferIdx);
}

void model::Quad::draw() const {
  glBindVertexArray(glAttributesIdx);
  glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(vertices.size()));
  glBindVertexArray(0);
}
