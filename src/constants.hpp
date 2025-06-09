#pragma once

#include <glm/glm.hpp>

#include "asset/asset.hpp"

namespace constants {
  constexpr float PI = 3.14159265358979323846f;
  constexpr float TAU = 2 * PI;

  constexpr glm::vec3 WORLD_ORIGIN = glm::vec3(0.0f, 0.0f, 0.0f);

  // Coordinate system matching Unreal Engine
  constexpr glm::vec3 WORLD_UP = glm::vec3(0.0f, 0.0f, 1.0f);
  constexpr glm::vec3 WORLD_FORWARD = glm::vec3(1.0f, 0.0f, 0.0f);
  constexpr glm::vec3 WORLD_RIGHT = glm::vec3(0.0f, 1.0f, 0.0f);

  constexpr glm::vec3 WORLD_GRAVITY = glm::vec3(0.0f, 0.0f, -9.81f);

  /* clang-format off */
  const asset::Material DEFAULT_MATERIAL_3D = {
    .name = "DefaultMaterial",
    .ambient = glm::vec3(0.05f),
    .diffuse = glm::vec3(0.8f, 0.8f, 0.8f),
    .specular = glm::vec3(0.5f),
    .shininess = 32.0f,
    .dissolve = 1.0f,
    .diffuseTexture = std::nullopt,
    .normalTexture = std::nullopt
  }; /* clang-format on */
};
