#pragma once

#include <glm/glm.hpp>

namespace components {
  struct LifeTime {
    float timeCreated;
    float lifeSpan;
  };

  struct Velocity {
    glm::vec3 velocity;
  };

  struct Acceleration {
    glm::vec3 acceleration;
  };
};
