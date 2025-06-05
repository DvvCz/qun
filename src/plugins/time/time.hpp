#pragma once

#include <entt/entt.hpp>

class Game;

namespace plugins {
  struct Time {
    void build(Game& game);
  };
};
