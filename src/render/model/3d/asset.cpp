#include "asset.hpp"

#include "render/vertex.hpp"
#include "render/material/material3d.hpp"

#include "asset/img/img.hpp"

#include <print>

model::Asset::Asset(/* clang-format off */
  const asset::Asset3D& asset,
  std::shared_ptr<TextureManager> texMan,
  std::shared_ptr<material::Manager3D> matMan
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

    glVertexArrayAttribFormat(glAttributesIdx, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, normal) - offsetof(Vertex3D, pos));
    glEnableVertexArrayAttrib(glAttributesIdx, 1);
    glVertexArrayAttribBinding(glAttributesIdx, 1, glAttrSlot1);

    glVertexArrayAttribFormat(glAttributesIdx, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex3D, uv) - offsetof(Vertex3D, normal));
    glEnableVertexArrayAttrib(glAttributesIdx, 2);
    glVertexArrayAttribBinding(glAttributesIdx, 2, glAttrSlot1);

    glVertexArrayVertexBuffer(glAttributesIdx, glAttrSlot1, glBufferIdx, 0, sizeof(Vertex3D));
    glVertexArrayElementBuffer(glAttributesIdx, glIndexBufferIdx); // Bind index buffer to VAO
  }

  for (const auto& shape : asset.shapes) {
    for (const auto& group : shape.groups) {
      allIndices.insert(allIndices.end(), group.indices.begin(), group.indices.end());
      materialGroups.push_back(group);
    }
  }

  std::filesystem::path basePath = asset.path.parent_path();
  for (const auto& material : asset.materials) {
    if (material.diffuseTexture.has_value()) {
      std::filesystem::path diffuseFileName = material.diffuseTexture.value();

      if (textureManager->getTextureByPath(diffuseFileName).has_value()) {
        std::println("Texture already loaded: {}", diffuseFileName.string());
        continue;
      }

      auto texture = asset::Img::tryFromFile(diffuseFileName);
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

  glNamedBufferData(glBufferIdx, sizeof(Vertex3D) * asset.vertices.size(), asset.vertices.data(), GL_STATIC_DRAW);
  glNamedBufferData(glIndexBufferIdx, sizeof(GLuint) * allIndices.size(), allIndices.data(), GL_STATIC_DRAW);
}

model::Asset::~Asset() {
  glDeleteVertexArrays(1, &glAttributesIdx);
  glDeleteBuffers(1, &glBufferIdx);
  glDeleteBuffers(1, &glIndexBufferIdx);
}

void model::Asset::draw() const {
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
