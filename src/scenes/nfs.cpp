#include "components/material.hpp"
#include "game.hpp"

#include <expected>
#include <string>

#include <glm/glm.hpp>

#include "components/transform.hpp"
#include "components/model.hpp"
#include "components/light.hpp"

#include "asset/obj/obj.hpp"

#include "render/model/3d/sphere.hpp"
#include "render/model/3d/cube.hpp"

#include "util/error.hpp"

std::expected<bool, std::string> Game::setupScene() noexcept {
  { // lamborghini
    auto asset = asset::loader::Gltf::tryFromFile("resources/lamborghini.glb", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load GLTF asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::scale(matrix, glm::vec3(0.5f));
    matrix = glm::translate(matrix, glm::vec3(-18.0f, 5.0f, 1.2f));
    matrix = glm::rotate(matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Rotate to face forward

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
  }

  { // car concept
    auto asset = asset::loader::Gltf::tryFromFile("resources/CarConcept/CarConcept.gltf", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load car asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::scale(matrix, glm::vec3(0.5f));
    matrix = glm::translate(matrix, glm::vec3(18.0f, 5.0f, 1.2f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
  }

  { // Ford GT40 Mark II.glb
    auto asset = asset::loader::Gltf::tryFromFile("resources/Ford GT40 Mark II.glb", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load Ford GT40 asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::scale(matrix, glm::vec3(0.5f));
    matrix = glm::translate(matrix, glm::vec3(0.0f, 5.0f, 1.2f));

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

  return true;
}
