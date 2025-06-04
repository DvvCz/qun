#include "particle.hpp"

#include "components/particle.hpp"

systems::Particle::Particle(const std::shared_ptr<entt::registry>& registry) : registry(registry) {
}

void systems::Particle::tick(float curTime, float dt) {
  auto hasLifetime = registry->view<components::LifeTime>();
  for (auto entity : hasLifetime) {
    auto& lifetime = hasLifetime.get<components::LifeTime>(entity);
    if ((lifetime.timeCreated + lifetime.lifeSpan) < curTime) {
      registry->destroy(entity);
    }
  }
}
