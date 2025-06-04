#include "boids.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../components/boid.hpp"
#include "../components/particle.hpp"
#include "../components/transform.hpp"

namespace systems {

  Boids::Boids(std::shared_ptr<entt::registry> registry) : registry(registry) {
  }

  void Boids::tick(float currentTime, float deltaTime) {
    // First pass: Calculate forces for all boids
    auto boidView = registry->view<components::Boid, components::Position, components::Velocity, components::BoidParams>();

    // Calculate forces for each boid
    for (auto entity : boidView) {
      auto position = boidView.get<components::Position>(entity);
      auto velocityComp = boidView.get<components::Velocity>(entity);
      auto params = boidView.get<components::BoidParams>(entity);

      glm::vec3 velocity = velocityComp.velocity;

      // Calculate the three main flocking forces
      glm::vec3 separation = calculateSeparation(entity, position, params.separationRadius);
      glm::vec3 alignment = calculateAlignment(entity, position, params.alignmentRadius);
      glm::vec3 cohesion = calculateCohesion(entity, position, params.cohesionRadius);

      // Apply weights to forces
      separation *= params.separationWeight;
      alignment *= params.alignmentWeight;
      cohesion *= params.cohesionWeight;

      // Add boundary force to keep boids in bounds
      glm::vec3 boundary = calculateBoundaryForce(position);

      // Combine all forces
      glm::vec3 totalForce = separation + alignment + cohesion + boundary;

      // Store forces in component for debugging/visualization if needed
      if (!registry->all_of<components::BoidForces>(entity)) {
        registry->emplace<components::BoidForces>(entity);
      }
      components::BoidForces forces;
      forces.separation = separation;
      forces.alignment = alignment;
      forces.cohesion = cohesion;
      forces.total = totalForce;
      registry->replace<components::BoidForces>(entity, forces);

      // Apply steering force with limits
      glm::vec3 steering = steer(totalForce, velocity, params.maxForce, params.maxSpeed);

      // Update velocity with steering force
      glm::vec3 newVelocity = velocity + steering * deltaTime;

      // Limit velocity to max speed
      if (glm::length(newVelocity) > params.maxSpeed) {
        newVelocity = glm::normalize(newVelocity) * params.maxSpeed;
      }

      // Update the velocity component in the registry
      components::Velocity newVel;
      newVel.velocity = newVelocity;
      registry->replace<components::Velocity>(entity, newVel);
    }
  }

  glm::vec3 Boids::calculateSeparation(entt::entity entity, const glm::vec3& position, float radius) {
    glm::vec3 steer{0.0f};
    int count = 0;

    auto boidView = registry->view<components::Boid, components::Position>();

    for (auto other : boidView) {
      if (other == entity)
        continue;

      auto& otherPos = boidView.get<components::Position>(other);
      glm::vec3 diff = position - otherPos;
      float distance = glm::length(diff);

      if (distance > 0.0f && distance < radius) {
        // Closer neighbors have stronger repulsion
        diff = glm::normalize(diff) / distance;
        steer += diff;
        count++;
      }
    }

    if (count > 0) {
      steer /= static_cast<float>(count);
      if (glm::length(steer) > 0.0f) {
        steer = glm::normalize(steer);
      }
    }

    return steer;
  }

  glm::vec3 Boids::calculateAlignment(entt::entity entity, const glm::vec3& position, float radius) {
    glm::vec3 avgVelocity{0.0f};
    int count = 0;

    auto boidView = registry->view<components::Boid, components::Position, components::Velocity>();

    for (auto other : boidView) {
      if (other == entity)
        continue;

      auto& otherPos = boidView.get<components::Position>(other);
      float distance = glm::length(position - otherPos);

      if (distance > 0.0f && distance < radius) {
        auto otherVel = boidView.get<components::Velocity>(other);
        avgVelocity += otherVel.velocity;
        count++;
      }
    }

    if (count > 0) {
      avgVelocity /= static_cast<float>(count);
      if (glm::length(avgVelocity) > 0.0f) {
        avgVelocity = glm::normalize(avgVelocity);
      }
    }

    return avgVelocity;
  }

  glm::vec3 Boids::calculateCohesion(entt::entity entity, const glm::vec3& position, float radius) {
    glm::vec3 center{0.0f};
    int count = 0;

    auto boidView = registry->view<components::Boid, components::Position>();

    for (auto other : boidView) {
      if (other == entity)
        continue;

      auto& otherPos = boidView.get<components::Position>(other);
      float distance = glm::length(position - otherPos);

      if (distance > 0.0f && distance < radius) {
        center += otherPos;
        count++;
      }
    }

    if (count > 0) {
      center /= static_cast<float>(count);
      glm::vec3 desired = center - position;
      if (glm::length(desired) > 0.0f) {
        desired = glm::normalize(desired);
      }
      return desired;
    }

    return glm::vec3{0.0f};
  }

  glm::vec3 Boids::steer(const glm::vec3& target, const glm::vec3& velocity, float maxForce, float maxSpeed) {
    glm::vec3 desired = target;

    if (glm::length(desired) > 0.0f) {
      desired = glm::normalize(desired) * maxSpeed;
      glm::vec3 steer = desired - velocity;

      if (glm::length(steer) > maxForce) {
        steer = glm::normalize(steer) * maxForce;
      }

      return steer;
    }

    return glm::vec3{0.0f};
  }

  glm::vec3 Boids::calculateBoundaryForce(const glm::vec3 position) {
    glm::vec3 force{0.0f};

    // Define boundary box (smaller area)
    const float boundarySize = 25.0f;
    const float boundaryForce = 5.0f;

    // Apply forces to keep boids within bounds
    if (position.x < -boundarySize) {
      force.x += boundaryForce;
    } else if (position.x > boundarySize) {
      force.x -= boundaryForce;
    }

    if (position.y < -boundarySize) {
      force.y += boundaryForce;
    } else if (position.y > boundarySize) {
      force.y -= boundaryForce;
    }

    if (position.z < -boundarySize) {
      force.z += boundaryForce;
    } else if (position.z > boundarySize) {
      force.z -= boundaryForce;
    }

    return force;
  }

} // namespace systems
