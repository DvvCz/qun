#pragma once

#include <entt/entt.hpp>
#include <memory>

namespace systems {
  class Transform {
  public:
    static void update(entt::registry& registry, entt::entity entity);
    static void construct(entt::registry& registry, entt::entity entity);

    Transform(const std::shared_ptr<entt::registry>& registry);

  private:
    std::shared_ptr<entt::registry> registry;
  };
};
