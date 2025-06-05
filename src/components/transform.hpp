#pragma once

#include <glm/glm.hpp>

namespace components {
  struct Position {
    glm::vec3 value;
  };

  // Use quat to avoid gimbal lock
  using Rotation = glm::quat;

  struct Scale {
    glm::vec3 value;
  };

  using GlobalTransform = glm::mat4x4;
};
