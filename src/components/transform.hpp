#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace components {
  struct Position {
    glm::vec3 value;
  };

  // Use quat to avoid gimbal lock
  struct Rotation {
    glm::quat value;
  };

  struct Scale {
    glm::vec3 value;
  };

  struct GlobalTransform {
    glm::mat4x4 value;
  };
};
