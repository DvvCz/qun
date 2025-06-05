#include "nfs.hpp"

#include <expected>
#include <string>

#include <glm/glm.hpp>

#include "components/transform.hpp"
#include "components/model.hpp"
#include "components/light.hpp"
#include "components/material.hpp"

#include "asset/obj/obj.hpp"
#include "asset/gltf/gltf.hpp"

#include "render/model/3d/sphere.hpp"
#include "render/model/3d/cube.hpp"

#include "util/error.hpp"

std::expected<void, std::string> scenes::nfs::startup(entt::registry& registry, Renderer& renderer) {
  { // car concept
    auto asset = asset::loader::Gltf::tryFromFile("resources/CarConcept/CarConcept.gltf", *renderer.textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load car asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer.createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::scale(matrix, glm::vec3(0.5f));
    matrix = glm::translate(matrix, glm::vec3(18.0f, 5.0f, 1.2f));

    auto ent = registry.create();
    registry.emplace<components::GlobalTransform>(ent, matrix);
    registry.emplace<components::Model3D>(ent, model);
  }

  { // main light
    auto matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, 5.0f));

    auto ent = registry.create();
    registry.emplace<components::GlobalTransform>(ent, matrix);
    registry.emplace<components::Light>(ent, glm::vec3(1.0f), 2.0f, 1000.0f);
  }

  return {};
}

std::expected<void, std::string> scenes::nfs::update(entt::registry& reg, Renderer& renderer) {
  return {};
}
