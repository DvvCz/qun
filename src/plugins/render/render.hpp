#pragma once

#include <entt/entt.hpp>

class Game;

namespace plugins {
  struct Render {
    void build(Game& game);
  };
};
