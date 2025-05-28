#pragma once

#include <glm/glm.hpp>

namespace constants {
  constexpr glm::vec3 WORLD_ORIGIN = glm::vec3(0.0f, 0.0f, 0.0f);

  // Coordinate system matching Unreal Engine
  constexpr glm::vec3 WORLD_UP = glm::vec3(0.0f, 0.0f, 1.0f);
  constexpr glm::vec3 WORLD_FORWARD = glm::vec3(1.0f, 0.0f, 0.0f);
  constexpr glm::vec3 WORLD_RIGHT = glm::vec3(0.0f, 1.0f, 0.0f);
};
