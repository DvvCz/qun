#include "physics.hpp"

#include "game.hpp"
#include "components/physics.hpp"
#include "components/transform.hpp"
#include "resources/time.hpp"

#include <entt/entt.hpp>

void plugins::Physics::build(Game& game) {
  // Add physics update system that runs during Update schedule
  game.addSystem(Schedule::Update, [](std::shared_ptr<entt::registry>& registry, resources::Time& time) {
    float deltaTime = time.deltaTime;

    // Update velocity based on acceleration
    registry->view<components::Velocity, components::Acceleration>().each(
        [&registry, deltaTime](entt::entity entity, auto& velocity, const auto& acceleration) {
          velocity.value += acceleration.value * deltaTime;
          // Use replace to trigger EnTT signals for velocity updates
          registry->replace<components::Velocity>(entity, velocity);
        });

    // Update position based on velocity
    registry->view<components::Position, components::Velocity>().each(
        [&registry, deltaTime](entt::entity entity, auto& position, const auto& velocity) {
          position.value += velocity.value * deltaTime;
          // Use replace to trigger EnTT signals for position updates (needed for transform system)
          registry->replace<components::Position>(entity, position);
        });
  });
}
