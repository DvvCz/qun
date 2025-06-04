#pragma once

#include <glm/glm.hpp>

namespace components {

  // Boid-specific parameters that control flocking behavior
  struct BoidParams {
    float separationRadius = 1.5f; // Distance to avoid other boids
    float alignmentRadius = 4.0f;  // Distance to align with other boids
    float cohesionRadius = 6.0f;   // Distance to move towards group center

    float separationWeight = 1.5f; // How strongly to avoid nearby boids
    float alignmentWeight = 1.2f;  // How strongly to align with neighbors
    float cohesionWeight = 1.8f;   // How strongly to move towards group (increased)

    float maxSpeed = 5.0f; // Maximum velocity magnitude (reduced)
    float maxForce = 3.0f; // Maximum steering force (reduced)
  };

  // Tag component to identify boids
  struct Boid {
    // Empty tag component - just marks entities as boids
  };

  // Steering forces applied to boids
  struct BoidForces {
    glm::vec3 separation{0.0f};
    glm::vec3 alignment{0.0f};
    glm::vec3 cohesion{0.0f};
    glm::vec3 total{0.0f};
  };

} // namespace components
