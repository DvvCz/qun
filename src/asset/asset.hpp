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

    // todo: just store the texture index
    std::optional<std::filesystem::path> diffuseTexture;
  };

  struct MaterialGroup {
  public:
    int materialId;
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
    int width;
    int height;
    int channels;
    std::vector<unsigned char> data;

    std::filesystem::path path;
  };
}
