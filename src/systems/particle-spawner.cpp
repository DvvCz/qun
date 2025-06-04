#include "particle-spawner.hpp"

#include <entt/entt.hpp>

#include "components/particle.hpp"
#include "components/transform.hpp"
#include "components/model.hpp"

#include "render/model/3d/cube.hpp"

systems::ParticleSpawner::ParticleSpawner(std::shared_ptr<entt::registry> registry) : registry(registry) {
  particleModel = std::make_shared<model::Cube>(glm::vec3(0.05f, 0.05f, 0.005f));
}

void systems::ParticleSpawner::tick(float curTime) {
  // Spawn a particle every 500ms (0.5 seconds)
  if (curTime - lastSpawnTime >= 0.05f) {
    auto ent = registry->create();
    registry->emplace<components::Model3D>(ent, particleModel);
    registry->emplace<components::Velocity>(ent, glm::vec3(sin(curTime) * 5.0f, cos(curTime) * 5.0f, 5.0f));
    registry->emplace<components::Acceleration>(ent, glm::vec3(0.0f, 0.0f, -9.81f));
    registry->emplace<components::LifeTime>(ent, curTime, 2.5f);
    registry->emplace<components::Position>(ent, glm::vec3(0.0f, 0.0f, 0.5f));

    lastSpawnTime = curTime;
  }
}
