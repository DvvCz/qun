#include "nfs.hpp"

#include <expected>
#include <print>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "components/physics.hpp"
#include "components/transform.hpp"
#include "components/model.hpp"
#include "components/light.hpp"
#include "components/material.hpp"

#include "game.hpp"

#include "asset/gltf/gltf.hpp"

#include "render/model/3d/cube.hpp"
#include "render/renderer.hpp"

#include "util/error.hpp"

#include "constants.hpp"

#include "input/raw/keyboard.hpp"
#include "resources/time.hpp"

static std::expected<void, std::string> startup(/* clang-format off */
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

    registry->emplace<components::Velocity>(ent, glm::vec3(0.0f, 0.0f, 0.0f));        // Start stationary
    registry->emplace<components::AngularVelocity>(ent, glm::vec3(0.0f, 0.0f, 0.0f)); // Start without rotation
  }

  { // baseplate
    auto model = std::make_shared<model::Cube>(glm::vec3(1000.0f, 1000.0f, 0.01f));

    auto ent = registry->create();
    registry->emplace<components::Position>(ent, constants::WORLD_ORIGIN);
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

static std::expected<void, std::string> update(/* clang-format off */
  std::shared_ptr<entt::registry> registry,
  std::shared_ptr<Renderer> renderer,
  resources::Time& time
) { /* clang-format on */

  // Car controller system
  auto carView =
      registry->view<components::Position, components::Rotation, components::Velocity, components::AngularVelocity>();

  for (auto entity : carView) {
    auto& position = carView.get<components::Position>(entity);
    auto& rotation = carView.get<components::Rotation>(entity);
    auto& velocity = carView.get<components::Velocity>(entity);
    auto& angularVelocity = carView.get<components::AngularVelocity>(entity);

    const float carSpeed = 5.0f;          // Units per second
    const float maxTurnSpeed = 2.0f;      // Maximum radians per second
    const float turnAcceleration = 8.0f;  // How fast we accelerate into turns
    const float turnDeceleration = 12.0f; // How fast we decelerate out of turns

    glm::vec3 inputVelocity(0.0f);
    float movementInput = 0.0f;         // Track how much we're moving
    float targetAngularVelocity = 0.0f; // Target turning speed

    glm::vec3 forward = rotation * glm::vec3(0.0f, 1.0f, 0.0f);

    // Forward/backward movement
    if (input::Keyboard::isCurrentlyHeld(input::Key::W)) {
      inputVelocity -= forward * carSpeed;
      movementInput = 1.0f; // Moving forward
    }
    if (input::Keyboard::isCurrentlyHeld(input::Key::S)) {
      inputVelocity += forward * carSpeed;
      movementInput = 1.0f; // Moving backward
    }

    // Steering (rotation around Z-axis) - only when moving
    if (movementInput > 0.0f) {
      if (input::Keyboard::isCurrentlyHeld(input::Key::A)) {
        targetAngularVelocity = maxTurnSpeed * movementInput; // Turn left
      }
      if (input::Keyboard::isCurrentlyHeld(input::Key::D)) {
        targetAngularVelocity = -maxTurnSpeed * movementInput; // Turn right
      }
    }

    // Smooth angular velocity interpolation
    float currentAngularZ = angularVelocity.value.z;
    float deltaAngular = targetAngularVelocity - currentAngularZ;

    if (std::abs(deltaAngular) > 0.01f) {
      float acceleration = (std::abs(targetAngularVelocity) > std::abs(currentAngularZ)) ? turnAcceleration : turnDeceleration;

      float maxChange = acceleration * time.deltaTime;
      if (std::abs(deltaAngular) > maxChange) {
        deltaAngular = (deltaAngular > 0) ? maxChange : -maxChange;
      }

      angularVelocity.value.z = currentAngularZ + deltaAngular;
    } else {
      angularVelocity.value.z = targetAngularVelocity;
    }

    // Apply the linear velocity
    velocity.value = inputVelocity;

    // Third-person camera system - follow the car from behind
    const float cameraDistance = 10.0f; // Distance behind the car
    const float cameraHeight = 3.0f;    // Height above the car

    // Calculate camera position: behind the car + height offset
    glm::vec3 cameraOffset = forward * cameraDistance + glm::vec3(0.0f, 0.0f, cameraHeight);
    glm::vec3 cameraPos = position.value + cameraOffset;

    // Camera looks at the car (slightly ahead of it)
    glm::vec3 lookTarget = position.value - forward * 2.0f;
    glm::vec3 cameraDir = glm::normalize(lookTarget - cameraPos);

    // Update renderer camera
    renderer->setCameraPos(cameraPos);
    renderer->setCameraDir(cameraDir);
  }

  return {};
}

void scenes::NFS::build(Game& game) {
  game.addSystem(Schedule::Startup, startup);
  game.addSystem(Schedule::Update, update);
}
