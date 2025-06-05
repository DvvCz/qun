#pragma once

#include <entt/entt.hpp>

class Game;

namespace plugins {
  struct Transform {
    void build(Game& game);
  };
};
