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
  struct ObjShape {
  public:
    std::string name;
    std::vector<int> indices;

    /// Material ids corresponding to each tri
    std::vector<int> materialIds;
  };

  class ObjAsset : public Asset {
  public:
    [[nodiscard]] static std::expected<ObjAsset, std::string> tryFromFile(const std::filesystem::path& path) noexcept;

    [[nodiscard]] std::optional<std::string> getName() const noexcept {
      return "OBJ Asset";
    }

    std::vector<Vertex> vertices;
    std::vector<ObjShape> shapes;
    std::vector<rapidobj::Material> materials;

    ObjAsset(std::vector<Vertex> vertices, std::vector<ObjShape> shapes, std::vector<rapidobj::Material> materials)
        : vertices(std::move(vertices)), shapes(std::move(shapes)), materials(std::move(materials)) {
    }
  };
}