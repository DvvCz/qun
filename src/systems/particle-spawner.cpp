#include "particle-spawner.hpp"

#include <entt/entt.hpp>

#include "components/material.hpp"
#include "components/particle.hpp"
#include "components/transform.hpp"
#include "components/model.hpp"

#include "render/model/3d/cube.hpp"
#include "render/model/3d/sphere.hpp"

systems::ParticleSpawner::ParticleSpawner(std::shared_ptr<entt::registry> registry) : registry(registry) {
  particleModel = std::make_shared<model::Sphere>(0.05f);

  particleMaterial = std::make_shared<asset::Material>();
  particleMaterial->diffuse = glm::vec3(0.0f, 0.0f, 0.4f);
}

void systems::ParticleSpawner::tick(float curTime) {
  // Spawn a particle every 500ms (0.5 seconds)
  if (curTime - lastSpawnTime >= 0.05f) {
    auto ent = registry->create();
    registry->emplace<components::Model3D>(ent, particleModel);
    registry->emplace<components::Material3D>(ent, particleMaterial);
    registry->emplace<components::Velocity>(ent, glm::vec3(sin(curTime) * 5.0f, cos(curTime) * 5.0f, 5.0f));
    registry->emplace<components::Acceleration>(ent, glm::vec3(0.0f, 0.0f, -9.81f));
    registry->emplace<components::LifeTime>(ent, curTime, 2.5f);
    registry->emplace<components::Position>(ent, glm::vec3(0.0f, 0.0f, 0.5f));

    lastSpawnTime = curTime;
  }
}
