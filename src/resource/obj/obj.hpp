#pragma once

#include <expected>
#include <string>
#include <vector>
#include <array>
#include <filesystem>
#include <optional>

#include <rapidobj/rapidobj.hpp>

#include "../asset.hpp"
#include "../../render/vertex.hpp"

namespace resource {
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

    std::optional<std::string> diffuseTexture;
  };

  class ObjAsset : public Asset {
  public:
    [[nodiscard]] std::optional<std::string> getName() const noexcept {
      return "OBJ Asset";
    }

    [[nodiscard]] static std::expected<ObjAsset, std::string> tryFromFile(const std::filesystem::path& path) noexcept;

    [[nodiscard]] std::filesystem::path getPath() const noexcept {
      return path;
    }

    std::vector<Vertex> vertices;
    std::vector<ObjShape> shapes;
    std::vector<ObjMaterial> materials;

    ObjAsset(std::vector<Vertex> vertices, std::vector<ObjShape> shapes, std::vector<ObjMaterial> materials,
             std::filesystem::path path)
        : vertices(std::move(vertices)), shapes(std::move(shapes)), materials(std::move(materials)), path(std::move(path)) {
    }

  private:
    std::filesystem::path path;
  };
}
