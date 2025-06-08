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

#include "game.hpp"

#include "asset/gltf/gltf.hpp"

#include "render/renderer.hpp"

#include "util/error.hpp"

#include "constants.hpp"

#include "input/raw/keyboard.hpp"
#include "input/raw/mouse.hpp"
#include "resources/time.hpp"

static std::expected<void, std::string> startup(/* clang-format off */
  std::shared_ptr<entt::registry> registry,
  std::shared_ptr<Renderer> renderer
) { /* clang-format on */
  {                                                                                 // car concept
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

    registry->emplace<components::Velocity>(ent, glm::vec3(0.0f, 0.0f, 0.0f));
    registry->emplace<components::AngularVelocity>(ent, glm::vec3(0.0f, 0.0f, 0.0f));
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
  std::shared_ptr<scenes::nfs::components::CameraState> cameraState,
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

    // GTA 5-style camera system
    auto carView = registry->view<components::Position, components::Rotation>();
    for (auto carEntity : carView) {
      auto& carPosition = carView.get<components::Position>(carEntity);
      auto& carRotation = carView.get<components::Rotation>(carEntity);

      // Get car forward direction
      glm::vec3 carForward = carRotation.value * glm::vec3(0.0f, 1.0f, 0.0f);

      // Handle mouse input for camera rotation
      glm::vec2 mouseDelta = input::Mouse::getPositionDelta();
      bool hasMouseInput = glm::length(mouseDelta) > 0.01f;

      if (hasMouseInput) {
        cameraState->yaw -= mouseDelta.x * cameraState->mouseSensitivity;
        cameraState->pitch += mouseDelta.y * cameraState->mouseSensitivity; // Fixed inversion

        // Clamp pitch to prevent camera flipping
        cameraState->pitch = std::clamp(cameraState->pitch, -1.4f, 0.5f); // About -80° to +30°

        cameraState->isUserControlling = true;
        cameraState->timeSinceLastInput = 0.0f;
      } else {
        cameraState->timeSinceLastInput += time.deltaTime;

        // Start auto-centering after delay
        if (cameraState->timeSinceLastInput > cameraState->autoReturnDelay) {
          cameraState->isUserControlling = false;

          // Calculate target yaw (behind the car)
          glm::vec3 carForwardXY = glm::normalize(glm::vec3(carForward.x, carForward.y, 0.0f));
          cameraState->targetYaw = atan2(carForwardXY.y, carForwardXY.x) + constants::PI; // Behind the car

          // Smoothly interpolate back to target position
          float returnSpeed = cameraState->autoReturnSpeed * time.deltaTime;

          // Handle yaw wrapping (shortest rotation path)
          float yawDiff = cameraState->targetYaw - cameraState->yaw;
          while (yawDiff > glm::pi<float>())
            yawDiff -= 2.0f * glm::pi<float>();
          while (yawDiff < -glm::pi<float>())
            yawDiff += 2.0f * glm::pi<float>();

          cameraState->yaw += yawDiff * returnSpeed;
          cameraState->pitch += (cameraState->targetPitch - cameraState->pitch) * returnSpeed;
        }
      }

      // Calculate camera position based on yaw, pitch, and distance
      float cosYaw = cos(cameraState->yaw);
      float sinYaw = sin(cameraState->yaw);
      float cosPitch = cos(cameraState->pitch);
      float sinPitch = sin(cameraState->pitch);

      // Camera offset in spherical coordinates (negative distance to position behind car)
      glm::vec3 cameraOffset;
      cameraOffset.x = -cameraState->distance * cosYaw * cosPitch;
      cameraOffset.y = -cameraState->distance * sinYaw * cosPitch;
      cameraOffset.z = cameraState->height + cameraState->distance * sinPitch;

      glm::vec3 cameraPos = carPosition.value + cameraOffset;

      // Camera looks at the car (with slight forward offset)
      glm::vec3 lookTarget = carPosition.value + carForward * 1.0f + glm::vec3(0.0f, 0.0f, 1.0f);
      glm::vec3 cameraDir = glm::normalize(lookTarget - cameraPos);

      // Update renderer camera
      renderer->setCameraPos(cameraPos);
      renderer->setCameraDir(cameraDir);

      // Only handle the first car entity for now
      break;
    }
  }

  return {};
}

void scenes::nfs::NFS::build(Game& game) {
  auto cameraState = std::make_shared<scenes::nfs::components::CameraState>();
  game.addResource(cameraState);

  game.addSystem(Schedule::Startup, startup);
  game.addSystem(Schedule::Update, update);
}
