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

  { // city
    auto asset = asset::loader::Gltf::tryFromFile("resources/City1.glb", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load city asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(0.0f, 0.0f, 0.0f));
    registry->emplace<components::Scale>(ent, glm::vec3(0.007f));
    registry->emplace<components::Model3D>(ent, model);
  }

  { // main light
    auto ent = registry->create();
    registry->emplace<components::Position>(ent, glm::vec3(0.0f, 0.0f, 5.0f));
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

    const float maxCarSpeed = 15.0f;       // Maximum speed in units per second
    const float acceleration = 25.0f;      // Acceleration rate
    const float deceleration = 30.0f;      // Deceleration rate when no input
    const float brakeDeceleration = 40.0f; // Deceleration when braking
    const float maxTurnSpeed = 2.0f;       // Maximum radians per second
    const float turnAcceleration = 8.0f;   // How fast we accelerate into turns
    const float turnDeceleration = 12.0f;  // How fast we decelerate out of turns

    float targetSpeed = 0.0f;
    float movementInput = 0.0f;         // Track how much we're moving
    float targetAngularVelocity = 0.0f; // Target turning speed

    glm::vec3 forward = rotation.value * glm::vec3(0.0f, 1.0f, 0.0f);

    // Get current speed in the forward direction
    float currentSpeed = glm::dot(velocity.value, -forward);

    // Forward/backward movement input
    if (input::Keyboard::isBeingHeld(input::Key::W)) {
      targetSpeed = maxCarSpeed;
      movementInput = 1.0f; // Moving forward
    } else if (input::Keyboard::isBeingHeld(input::Key::S)) {
      targetSpeed = -maxCarSpeed * 0.6f; // Reverse is slower
      movementInput = 1.0f;              // Moving backward
    }

    // Smooth speed interpolation
    float speedDifference = targetSpeed - currentSpeed;
    float maxSpeedChange;

    if (movementInput > 0.0f) {
      // We have input - accelerate towards target
      if ((targetSpeed > 0 && currentSpeed < targetSpeed) || (targetSpeed < 0 && currentSpeed > targetSpeed)) {
        maxSpeedChange = acceleration * time.deltaTime;
      } else {
        // Braking (changing direction or going too fast)
        maxSpeedChange = brakeDeceleration * time.deltaTime;
      }
    } else {
      // No input - natural deceleration
      maxSpeedChange = deceleration * time.deltaTime;
    }

    // Apply speed change with limits
    if (std::abs(speedDifference) > maxSpeedChange) {
      currentSpeed += (speedDifference > 0) ? maxSpeedChange : -maxSpeedChange;
    } else {
      currentSpeed = targetSpeed;
    }

    // Convert speed back to velocity vector
    velocity.value = -forward * currentSpeed;

    // Steering (rotation around Z-axis) - scale with speed for more realistic handling
    float speedFactor = std::min(std::abs(currentSpeed) / maxCarSpeed, 1.0f);
    if (speedFactor > 0.1f) { // Only allow steering when moving with some speed
      if (input::Keyboard::isBeingHeld(input::Key::A)) {
        targetAngularVelocity = maxTurnSpeed * speedFactor; // Turn left
      }
      if (input::Keyboard::isBeingHeld(input::Key::D)) {
        targetAngularVelocity = -maxTurnSpeed * speedFactor; // Turn right
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

void scenes::nfs::NFS::build(Game& game) {
  std::shared_ptr<scenes::nfs::components::CameraState> cameraState;
  game.addResource(cameraState);

  game.addSystem(Schedule::Startup, startup);
  game.addSystem(Schedule::Update, update);
}
