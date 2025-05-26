#pragma once

#include <glm/glm.hpp>

namespace components {
  struct Light {
    glm::vec3 color;
    float intensity;
    float radius;
  };
};
