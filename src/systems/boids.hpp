#pragma once

#include <memory>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace systems {

  class Boids {
  public:
    explicit Boids(std::shared_ptr<entt::registry> registry);
    ~Boids() = default;

    // Update boids simulation
    void tick(float currentTime, float deltaTime);

  private:
    std::shared_ptr<entt::registry> registry;

    // Calculate separation force - avoid nearby boids
    glm::vec3 calculateSeparation(entt::entity entity, const glm::vec3& position, float radius);

    // Calculate alignment force - align with nearby boids' velocities
    glm::vec3 calculateAlignment(entt::entity entity, const glm::vec3& position, float radius);

    // Calculate cohesion force - move towards group center
    glm::vec3 calculateCohesion(entt::entity entity, const glm::vec3& position, float radius);

    // Apply steering force with limits
    glm::vec3 steer(const glm::vec3& target, const glm::vec3& velocity, float maxForce, float maxSpeed);

    // Keep boids within bounds
    glm::vec3 calculateBoundaryForce(const glm::vec3 position);
  };

} // namespace systems
