#pragma once

#include <optional>
#include <string>
#include <vector>
#include <filesystem>

#include <glm/glm.hpp>

#include "render/vertex.hpp"

namespace asset {
  struct Material {
  public:
    std::string name;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    float dissolve;

    std::optional<size_t> diffuseTexture;
  };

  struct MaterialGroup {
  public:
    // Index into the Material[] in Asset3D
    size_t materialId;
    std::vector<int> indices;
  };

  struct Shape {
  public:
    std::string name;
    std::vector<MaterialGroup> groups;
  };

  struct Asset3D {
    std::vector<Vertex3D> vertices;
    std::vector<Shape> shapes;
    std::vector<Material> materials;

    std::filesystem::path path;
  };

  struct Asset2D {
    // TextureID to whatever texture manager this is linked to.
    size_t textureId;
  };
}
