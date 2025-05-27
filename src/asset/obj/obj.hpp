#pragma once

#include <expected>
#include <string>
#include <vector>
#include <filesystem>
#include <optional>

#include <rapidobj/rapidobj.hpp>
#include <glm/glm.hpp>

#include "asset/asset.hpp"
#include "render/vertex.hpp"

namespace asset {
  struct ObjMaterialGroup {
  public:
    int materialId;
    std::vector<int> indices;
  };

  struct ObjShape {
  public:
    std::string name;
    std::vector<ObjMaterialGroup> groups;
  };

  struct ObjMaterial {
  public:
    std::string name;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
    float dissolve;

    std::optional<std::filesystem::path> diffuseTexture;
  };

  class Obj : public Asset {
  public:
    [[nodiscard]] std::optional<std::string> getName() const noexcept {
      return "OBJ Asset";
    }

    [[nodiscard]] static std::expected<Obj, std::string> tryFromFile(const std::filesystem::path& path) noexcept;

    [[nodiscard]] std::filesystem::path getPath() const noexcept {
      return path;
    }

    std::vector<Vertex3D> vertices;
    std::vector<ObjShape> shapes;
    std::vector<ObjMaterial> materials;

    Obj(std::vector<Vertex3D> vertices, std::vector<ObjShape> shapes, std::vector<ObjMaterial> materials,
        std::filesystem::path path)
        : vertices(std::move(vertices)), shapes(std::move(shapes)), materials(std::move(materials)), path(std::move(path)) {
    }

  private:
    std::filesystem::path path;
  };
}
