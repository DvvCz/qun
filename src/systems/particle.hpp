#pragma once

#include <entt/entt.hpp>
#include <memory>

namespace systems {
  class Particle {
  public:
    void tick(float curTime, float dt);

    Particle(const std::shared_ptr<entt::registry>& registry);

  private:
    std::shared_ptr<entt::registry> registry;
  };
};
