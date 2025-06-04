#pragma once

#include <memory>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "asset/asset.hpp"
#include "render/model/model.hpp"

namespace systems {

  class BoidSpawner {
  public:
    explicit BoidSpawner(std::shared_ptr<entt::registry> registry);
    ~BoidSpawner() = default;

    void tick(float currentTime);

    // Spawn a group of boids at a specific location
    void spawnBoidGroup(const glm::vec3& center, int count = 20, float spread = 5.0f);

  private:
    std::shared_ptr<entt::registry> registry;

    // Shared model and material for all boids
    std::shared_ptr<Model3D> boidModel;
    std::shared_ptr<asset::Material> boidMaterial;

    float lastSpawnTime = 0.0f;
    bool hasSpawnedInitial = false;

    // Spawn a single boid at the specified position
    entt::entity spawnBoid(const glm::vec3& position, const glm::vec3& velocity = glm::vec3{0.0f});
  };

} // namespace systems
