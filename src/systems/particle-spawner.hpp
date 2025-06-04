#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "render/model/model.hpp"

namespace systems {
  class ParticleSpawner {
  public:
    void tick(float curTime);

    ParticleSpawner(std::shared_ptr<entt::registry> registry);

  private:
    std::shared_ptr<entt::registry> registry;
    std::shared_ptr<Model3D> particleModel;
    float lastSpawnTime = -1.0f;
  };
};
