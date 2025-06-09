#pragma once

#include <glm/glm.hpp>

namespace components {
  struct Velocity {
    glm::vec3 value;
  };

  struct Acceleration {
    glm::vec3 value;
  };

  struct AngularVelocity {
    glm::vec3 value;
  };

  struct AngularAcceleration {
    glm::vec3 value;
  };

  // Box collider for collision detection
  struct BoxCollider {
    glm::vec3 halfExtents;
  };
};
