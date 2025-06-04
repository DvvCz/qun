#pragma once

#include <entt/entt.hpp>
#include <memory>

#include "asset/asset.hpp"
#include "render/model/model.hpp"

namespace systems {
  class ParticleSpawner {
  public:
    void tick(float curTime);

    ParticleSpawner(std::shared_ptr<entt::registry> registry);

  private:
    std::shared_ptr<entt::registry> registry;
    std::shared_ptr<Model3D> particleModel;
    std::shared_ptr<asset::Material> particleMaterial;
    float lastSpawnTime = -1.0f;
  };
};
