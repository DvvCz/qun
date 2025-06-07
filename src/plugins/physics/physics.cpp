#include "physics.hpp"

#include "game.hpp"
#include "components/physics.hpp"
#include "components/transform.hpp"
#include "resources/time.hpp"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

void plugins::Physics::build(Game& game) {
  game.addSystem(Schedule::Update, [](std::shared_ptr<entt::registry>& registry, resources::Time& time) {
    float deltaTime = time.deltaTime;

    registry->view<components::AngularVelocity, components::AngularAcceleration>().each(
        [&registry, deltaTime](entt::entity entity, auto& angularVelocity, const auto& angularAcceleration) {
          angularVelocity.value += angularAcceleration.value * deltaTime;
          registry->replace<components::AngularVelocity>(entity, angularVelocity);
        });

    registry->view<components::Velocity, components::Acceleration>().each(
        [&registry, deltaTime](entt::entity entity, auto& velocity, const auto& acceleration) {
          velocity.value += acceleration.value * deltaTime;
          registry->replace<components::Velocity>(entity, velocity);
        });

    // Update rotation based on angular velocity
    registry->view<components::Rotation, components::AngularVelocity>().each(
        [&registry, deltaTime](entt::entity entity, auto& rotation, const auto& angularVelocity) {
          glm::vec3 angularVel = angularVelocity.value;
          float angle = glm::length(angularVel) * deltaTime;

          if (angle > 0.0f) {
            glm::vec3 axis = glm::normalize(angularVel);
            glm::quat deltaRotation = glm::angleAxis(angle, axis);
            rotation.value = deltaRotation * rotation.value;
            registry->replace<components::Rotation>(entity, rotation);
          }
        });

    registry->view<components::Position, components::Velocity>().each(
        [&registry, deltaTime](entt::entity entity, auto& position, const auto& velocity) {
          position.value += velocity.value * deltaTime;
          registry->replace<components::Position>(entity, position);
        });
  });
}
