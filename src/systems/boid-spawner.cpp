#include "boid-spawner.hpp"

#include <random>
#include <glm/gtc/constants.hpp>

#include "../components/boid.hpp"
#include "../components/particle.hpp"
#include "../components/transform.hpp"
#include "../components/material.hpp"
#include "../components/model.hpp"

#include "../render/model/3d/sphere.hpp"

namespace systems {

  BoidSpawner::BoidSpawner(std::shared_ptr<entt::registry> registry) : registry(registry) {
    // Create a smaller sphere for boids (smaller than particles)
    boidModel = std::make_shared<model::Sphere>(0.03f);

    // Create a distinct material for boids (reddish color to distinguish from blue particles)
    boidMaterial = std::make_shared<asset::Material>();
    boidMaterial->diffuse = glm::vec3(0.8f, 0.2f, 0.2f); // Red color for boids
  }

  void BoidSpawner::tick(float currentTime) {
    // Spawn initial boids when the system starts
    if (!hasSpawnedInitial) {
      // Spawn a few groups of boids in different locations with tighter clustering
      spawnBoidGroup(glm::vec3{0.0f, 0.0f, 0.0f}, 25, 4.0f);
      spawnBoidGroup(glm::vec3{15.0f, 10.0f, 5.0f}, 20, 3.0f);
      spawnBoidGroup(glm::vec3{-10.0f, 15.0f, -5.0f}, 20, 3.0f);

      hasSpawnedInitial = true;
      lastSpawnTime = currentTime;
    }

    // Optional: Spawn additional boids periodically
    // Uncomment the following lines if you want continuous spawning
    /*
    const float spawnInterval = 10.0f; // Spawn every 10 seconds
    if (currentTime - lastSpawnTime > spawnInterval) {
      spawnBoidGroup(glm::vec3{
        (std::rand() % 40) - 20.0f,
        (std::rand() % 40) - 20.0f,
        (std::rand() % 20) - 10.0f
      }, 15, 4.0f);
      lastSpawnTime = currentTime;
    }
    */
  }

  void BoidSpawner::spawnBoidGroup(const glm::vec3& center, int count, float spread) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDist(-spread, spread);
    std::uniform_real_distribution<float> velDist(-0.5f, 0.5f); // Reduced initial velocity

    for (int i = 0; i < count; ++i) {
      // Random position within spread around center
      glm::vec3 position = center + glm::vec3{posDist(gen), posDist(gen), posDist(gen)};

      // Random initial velocity (much smaller)
      glm::vec3 velocity{velDist(gen), velDist(gen), velDist(gen)};

      spawnBoid(position, velocity);
    }
  }

  entt::entity BoidSpawner::spawnBoid(const glm::vec3& position, const glm::vec3& velocity) {
    auto entity = registry->create();

    // Add boid tag component
    registry->emplace<components::Boid>(entity);

    // Add rendering components
    registry->emplace<components::Model3D>(entity, boidModel);
    registry->emplace<components::Material3D>(entity, boidMaterial);

    // Add transform components (reusing particle components)
    registry->emplace<components::Position>(entity, position);
    registry->emplace<components::Velocity>(entity, velocity);
    registry->emplace<components::Acceleration>(entity, glm::vec3{0.0f});

    // Add boid-specific parameters
    components::BoidParams params;

    // Add some variation to boid parameters for more interesting behavior
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> paramVariation(0.8f, 1.2f);

    params.separationRadius *= paramVariation(gen);
    params.alignmentRadius *= paramVariation(gen);
    params.cohesionRadius *= paramVariation(gen);
    params.maxSpeed *= paramVariation(gen);

    registry->emplace<components::BoidParams>(entity, params);

    // Add forces component for debugging
    registry->emplace<components::BoidForces>(entity);

    // Add scale and rotation for rendering (if needed)
    // registry->emplace<components::Scale>(entity, glm::vec3{0.2f}); // Make boids smaller than particles
    // registry->emplace<components::Rotation>(entity, glm::vec3{0.0f});

    return entity;
  }

} // namespace systems
