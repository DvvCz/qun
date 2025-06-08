#pragma once

#include <optional>
#include <string>
#include <vector>
#include <filesystem>

#include <glm/glm.hpp>

#include "render/texture.hpp"
#include "render/vertex.hpp"

namespace asset {
  struct Material {
  public:
    std::string name;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    float dissolve = 1.0f;
    bool isDoubleSided = true;
    glm::vec3 emissive;
    float emissiveStrength;

    std::optional<texture::Texture> diffuseTexture;
    std::optional<texture::Texture> normalTexture;
    std::optional<texture::Texture> emissiveTexture;
  };

  struct MaterialGroup {
  public:
    // Index into the Material[] in Asset3D
    std::optional<size_t> materialId;
    std::vector<int> indices;
  };

  struct Node {
  public:
    std::string name;
    std::vector<size_t> children; // Indices into the Node[] in Asset3D
    std::vector<MaterialGroup> groups;
  };

  struct Asset3D {
    std::vector<Vertex3D> vertices;
    std::vector<Node> nodes;
    std::vector<size_t> rootNodes; // Indices into the Node[] for root nodes
    std::vector<Material> materials;

    std::filesystem::path path;
  };

  struct Asset2D {
    texture::Texture texture;
  };
}
