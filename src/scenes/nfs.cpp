#include "nfs.hpp"

#include <expected>
#include <print>
#include <string>

#include <glm/glm.hpp>

#include "components/physics.hpp"
#include "components/transform.hpp"
#include "components/model.hpp"
#include "components/light.hpp"
#include "components/material.hpp"

#include "asset/obj/obj.hpp"
#include "asset/gltf/gltf.hpp"

#include "render/model/3d/sphere.hpp"
#include "render/model/3d/cube.hpp"

#include "util/error.hpp"

#include "constants.hpp"

std::expected<void, std::string> scenes::nfs::startup(/* clang-format off */
  std::shared_ptr<entt::registry> registry,
  std::shared_ptr<Renderer> renderer
) { /* clang-format on */

  { // car concept
    auto asset = asset::loader::Gltf::tryFromFile("resources/CarConcept/CarConcept.gltf", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load car asset: {}", util::error::indent(asset.error()))};
    }

    // TODO: For some reason gltf loader doesn't work with this material
    // Will need to resolve it properly later.
    for (auto& material : asset->materials) {
      if (material.name == "Glass") {
        material.dissolve = 0.3f;
      }
    }

    auto model = renderer->createAsset3D(asset.value());

    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(0.0f, 0.0f, 0.0f));
    registry->emplace<components::Model3D>(ent, model);
  }

  { // baseplate
    auto model = std::make_shared<model::Cube>(glm::vec3(1000.0f, 1000.0f, 0.01f));

    auto matrix = glm::mat4(1.0f);

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
  }

  { // main light
    auto matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, 5.0f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Light>(ent, glm::vec3(1.0f), 2.0f, 1000.0f);
  }

  return {};
}

std::expected<void, std::string> scenes::nfs::update(/* clang-format off */
  std::shared_ptr<entt::registry> registry,
  std::shared_ptr<Renderer> renderer
) { /* clang-format on */
  return {};
}
