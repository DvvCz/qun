#include "model.hpp"
#include "texture.hpp"
#include "material.hpp"

TriangleModel::TriangleModel(Vertex v1, Vertex v2, Vertex v3) {
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

AssetModel::AssetModel(/* clang-format off */
  const resource::ObjAsset& asset,
  std::shared_ptr<TextureManager> texMan,
  std::shared_ptr<MaterialManager> matMan
):
  inner(asset),
  textureManager(texMan),
  materialManager(matMan)
{/* clang-format on */
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
    glVertexArrayElementBuffer(glAttributesIdx, glIndexBufferIdx); // Bind index buffer to VAO
  }

  for (const auto& shape : asset.shapes) {
    allIndices.insert(allIndices.end(), shape.indices.begin(), shape.indices.end());
  }

  glNamedBufferData(glBufferIdx, sizeof(Vertex) * asset.vertices.size(), asset.vertices.data(), GL_STATIC_DRAW);
  glNamedBufferData(glIndexBufferIdx, sizeof(GLuint) * allIndices.size(), allIndices.data(), GL_STATIC_DRAW);
}

AssetModel::~AssetModel() {
  glDeleteVertexArrays(1, &glAttributesIdx);
  glDeleteBuffers(1, &glBufferIdx);
  glDeleteBuffers(1, &glIndexBufferIdx);
}

void AssetModel::draw() const {
  glBindVertexArray(glAttributesIdx);
  glDrawElements(GL_TRIANGLES, allIndices.size(), GL_UNSIGNED_INT, nullptr);
}
