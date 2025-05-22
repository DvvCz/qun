#include "model.hpp"
#include "texture.hpp"
#include "material.hpp"

#include <print>

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
    for (const auto& group : shape.groups) {
      allIndices.insert(allIndices.end(), group.indices.begin(), group.indices.end());
      materialGroups.push_back(group);
    }
  }

  std::filesystem::path basePath = asset.getPath().parent_path();
  for (const auto& material : asset.materials) {
    if (material.diffuseTexture.has_value()) {
      std::filesystem::path diffuseFileName = material.diffuseTexture.value();

      // Paths are relative to the asset itself
      std::filesystem::path resolvedPath = basePath / diffuseFileName;

      if (textureManager->getTextureByPath(diffuseFileName).has_value()) {
        std::println("Texture already loaded: {}", resolvedPath.string());
        continue;
      }

      auto texture = resource::ImgAsset::tryFromFile(resolvedPath);
      if (texture.has_value()) {
        auto textureId = textureManager->addTexture(texture.value());
        if (!textureId.has_value()) {
          std::println(stderr, "Failed to add texture: {}", textureId.error());
        }
      } else {
        std::println(stderr, "Failed to load texture: {}", texture.error());
      }
    }
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

  size_t currentOffset = 0;
  for (const auto& group : materialGroups) {
    if (group.materialId >= 0 && group.materialId < static_cast<int>(inner.materials.size())) {
      const auto& material = inner.materials[group.materialId];

      materialManager->setMaterial(material);
    }

    glDrawElements(GL_TRIANGLES, group.indices.size(), GL_UNSIGNED_INT, (void*)currentOffset);
    currentOffset += group.indices.size() * sizeof(GLuint);
  }
}
