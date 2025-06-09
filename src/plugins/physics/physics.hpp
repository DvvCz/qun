#pragma once

class Game;

#include <glm/glm.hpp>
#include <functional>
#include <entt/entt.hpp>

namespace plugins {
  namespace physics::components {
    struct CollisionEvent {
      entt::entity entity1;
      entt::entity entity2;
      glm::vec3 penetrationDepth;
    };

    struct CollisionCallback {
      std::function<void(const CollisionEvent&)> onCollisionEnter;
      std::function<void(const CollisionEvent&)> onCollisionStay;
      std::function<void(const CollisionEvent&)> onCollisionExit;
    };
  };

  struct Physics {
    void build(Game& game);

  private:
    static bool checkAABBCollision(const glm::vec3& pos1, const glm::vec3& halfExtents1, const glm::vec3& pos2,
                                   const glm::vec3& halfExtents2, glm::vec3& penetrationDepth);
  };
};
