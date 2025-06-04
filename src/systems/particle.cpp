#include "particle.hpp"

#include "components/particle.hpp"
#include "components/transform.hpp"

systems::Particle::Particle(const std::shared_ptr<entt::registry> registry) : registry(registry) {
}

void systems::Particle::tick(float curTime, float dt) {
  auto hasLifetime = registry->view<components::LifeTime>();
  for (auto entity : hasLifetime) {
    auto& lifetime = hasLifetime.get<components::LifeTime>(entity);
    if ((lifetime.timeCreated + lifetime.lifeSpan) < curTime) {
      registry->destroy(entity);
    }
  }

  auto ents = registry->view<components::Velocity, components::Acceleration>();
  for (auto& ent : ents) {
    auto& velocity = ents.get<components::Velocity>(ent);
    auto& acceleration = ents.get<components::Acceleration>(ent);

    registry->replace<components::Velocity>(ent, velocity.velocity + acceleration.acceleration * dt);

    // Update position based on velocity: p = p + v * dt
    // Check if the entity has a position component
    if (registry->all_of<components::Position>(ent)) {
      auto& position = registry->get<components::Position>(ent);
      registry->replace<components::Position>(ent, position + velocity.velocity * dt);
    }
  }
}
