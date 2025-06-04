#include "particle-spawner.hpp"

#include <entt/entt.hpp>

#include "components/particle.hpp"
#include "components/transform.hpp"

#include "render/model/3d/cube.hpp"

systems::ParticleSpawner::ParticleSpawner(std::shared_ptr<entt::registry> registry) : registry(registry) {
  particleModel = std::make_shared<model::Cube>(glm::vec3(10.0f, 10.0f, 1.0f));
}

void systems::ParticleSpawner::tick(float curTime, float dt) {
  uint64_t curTimeMs = static_cast<uint64_t>(curTime * 1000.0);

  if (curTimeMs % 500) {
    auto ent = registry->create();
    registry->emplace<components::Velocity>(ent, glm::vec3(0.0f, 0.0f, 1.0f));
    registry->emplace<components::Acceleration>(ent, glm::vec3(0.0f, 0.0f, -9.81f));
    registry->emplace<components::LifeTime>(ent, curTime, 5.0f);
    registry->emplace<components::Position>(ent, glm::vec3(0.0f, 0.0f, 0.5f));
  }
}
