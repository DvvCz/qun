#pragma once

#include <glm/glm.hpp>

namespace components {
  using Position = glm::vec3;

  // Use quat to avoid gimbal lock
  using Rotation = glm::quat;

  using Scale = glm::vec3;

  using GlobalTransform = glm::mat4x4;
};
