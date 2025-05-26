#pragma once

#include <glm/glm.hpp>

namespace components {
  using Position = glm::vec3;
  using Rotation = glm::vec3;
  using Scale = glm::vec3;

  using GlobalTransform = glm::mat4x4;
};
