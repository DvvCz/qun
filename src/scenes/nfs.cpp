#include "nfs.hpp"
#include "components/parent.hpp"
#include "render/model/3d/cube.hpp"
#include "render/model/3d/sphere.hpp"

#include <expected>
#include <glm/ext/quaternion_trigonometric.hpp>
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
#include "asset/img/img.hpp"

#include "render/renderer.hpp"

#include "util/error.hpp"

#include "constants.hpp"

#include "input/raw/keyboard.hpp"
#include "input/raw/mouse.hpp"
#include "resources/time.hpp"

// Function to recursively traverse nodes and create lights for emissive materials
static void createLightsForEmissiveMaterials(const asset::Asset3D& cityAsset, std::shared_ptr<entt::registry> registry,
                                             const glm::vec3& baseScale = glm::vec3(0.007f),
                                             const glm::vec3& basePosition = glm::vec3(0.0f)) {
  // Helper function to traverse a node recursively
  std::function<void(size_t)> traverseNode = [&](size_t nodeIndex) {
    if (nodeIndex >= cityAsset.nodes.size()) {
      return;
    }

    const auto& node = cityAsset.nodes[nodeIndex];

    // Check each material group in this node
    for (const auto& materialGroup : node.groups) {
      if (!materialGroup.materialId.has_value() || materialGroup.indices.empty()) {
        continue;
      }

      size_t materialIndex = materialGroup.materialId.value();
      if (materialIndex >= cityAsset.materials.size()) {
        continue;
      }

      const auto& material = cityAsset.materials[materialIndex];

      // Check if this material has an emissive texture
      if (material.emissiveTexture.has_value()) {
        // Calculate the centroid of vertices in this material group
        glm::vec3 centroid(0.0f);
        int validVertexCount = 0;

        for (int index : materialGroup.indices) {
          if (index >= 0 && index < static_cast<int>(cityAsset.vertices.size())) {
            centroid += cityAsset.vertices[index].pos;
            validVertexCount++;
          }
        }

        if (validVertexCount > 0) {
          centroid /= static_cast<float>(validVertexCount);

          // Apply the scaling and positioning transformation that's applied to the city model
          glm::vec3 lightPosition = centroid * baseScale + basePosition;

          // Create a light entity at this position
          auto lightEntity = registry->create();
          registry->emplace<components::Position>(lightEntity, lightPosition);

          // Set light properties based on the material
          // Use a warm color for emissive materials and moderate intensity
          glm::vec3 lightColor = material.emissive;
          float intensity = material.emissiveStrength;
          float radius = 2.0f; // Smaller radius since city is scaled down

          registry->emplace<components::Light>(lightEntity, lightColor, intensity, radius);
        }
      }
    }

    // Recursively process child nodes
    for (size_t childIndex : node.children) {
      traverseNode(childIndex);
    }
  };

  // Start traversal from all root nodes
  for (size_t rootNodeIndex : cityAsset.rootNodes) {
    traverseNode(rootNodeIndex);
  }
}

static std::expected<void, std::string> startup(/* clang-format off */
  std::shared_ptr<entt::registry> registry,
  std::shared_ptr<Renderer> renderer
) {
  { // baseplate 1000x1000 (invisible)
    auto baseplateEnt = registry->create();
    registry->emplace<components::Position>(baseplateEnt, glm::vec3(0.0f, 0.0f, -0.2f));
    registry->emplace<components::BoxCollider>(baseplateEnt, glm::vec3(500.0f, 500.0f, 0.05f));
  }

  {
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
    registry->emplace<scenes::nfs::components::Car>(ent); // mark as car entity
    registry->emplace<components::BoxCollider>(ent, glm::vec3(0.5f, 0.5f, 0.20f));
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

    createLightsForEmissiveMaterials(asset.value(), registry);
  }

  { // skybox (3d sphere)
    auto img = asset::loader::Img::tryFromFile("resources/ClearNight.png", *renderer->textureManager3D);
    if (!img.has_value()) {
      return std::unexpected{std::format("Failed to load skybox image: {}", util::error::indent(img.error()))};
    }

    auto skyboxModel = std::make_shared<model::Sphere>(1000.0f);

    auto skyboxMaterial = std::make_shared<asset::Material>();
    skyboxMaterial->ambient = glm::vec3(0.5f);
    skyboxMaterial->diffuseTexture = img.value().texture;

    auto skyboxEnt = registry->create();
    registry->emplace<components::Position>(skyboxEnt, glm::vec3(0.0f, 0.0f, -100.0f));
    registry->emplace<components::Rotation>(skyboxEnt, glm::angleAxis(glm::radians(90.0f), constants::WORLD_FORWARD));
    registry->emplace<components::Model3D>(skyboxEnt, skyboxModel);
    registry->emplace<components::Material3D>(skyboxEnt, skyboxMaterial);
  }

  return {};
}

static std::expected<void, std::string> update(/* clang-format off */
  std::shared_ptr<entt::registry> registry,
  std::shared_ptr<Renderer> renderer,
  std::shared_ptr<scenes::nfs::resources::CameraState> cameraState,
  resources::Time& time
) { /* clang-format on */

  if (input::Mouse::wasJustPressed(input::MouseButton::Left)) {
    auto boxAsset = std::make_shared<model::Cube>(glm::vec3(1.0f));

    // Create the box entity
    auto boxEntity = registry->create();
    registry->emplace<components::Position>(boxEntity, glm::vec3(0.0f, 0.0f, 20.0f));
    // registry->emplace<components::Scale>(boxEntity, glm::vec3(0.03f));
    registry->emplace<components::Model3D>(boxEntity, boxAsset);
    registry->emplace<components::Velocity>(boxEntity, glm::vec3(0.0f, 0.0f, 0.0f));
    registry->emplace<components::Acceleration>(boxEntity, constants::WORLD_GRAVITY);
    registry->emplace<components::BoxCollider>(boxEntity, glm::vec3(0.5f));
  }

  // Car controller system
  auto carView = registry->view<components::Position, components::Rotation, components::Velocity, components::AngularVelocity,
                                scenes::nfs::components::Car>();

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

    // GTA 5-style camera system - use the current car entity
    auto& carPosition = position; // Use the current car's position
    auto& carRotation = rotation; // Use the current car's rotation

    // Get car forward direction
    glm::vec3 carForward = carRotation.value * glm::vec3(0.0f, 1.0f, 0.0f);

    // Handle mouse input for camera rotation
    glm::vec2 mouseDelta = input::Mouse::getPositionDelta();
    bool hasMouseInput = glm::length(mouseDelta) > 0.01f;

    if (hasMouseInput) {
      // Apply mouse sensitivity with delta time for frame-rate independent movement
      float deltaTimeSensitivity = cameraState->mouseSensitivity * time.deltaTime;
      cameraState->yaw -= mouseDelta.x * deltaTimeSensitivity;
      cameraState->pitch += mouseDelta.y * deltaTimeSensitivity; // Fixed inversion

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

  return {};
}

void scenes::nfs::NFS::build(Game& game) {
  auto cameraState = std::make_shared<scenes::nfs::resources::CameraState>();
  game.addResource(cameraState);

  game.addSystem(Schedule::Startup, startup);
  game.addSystem(Schedule::Update, update);
}
