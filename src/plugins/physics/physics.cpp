#include "physics.hpp"

#include "game.hpp"
#include "components/physics.hpp"
#include "components/transform.hpp"
#include "resources/time.hpp"

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <vector>
#include <print>

bool plugins::Physics::checkAABBCollision(const glm::vec3& pos1, const glm::vec3& halfExtents1, const glm::vec3& pos2,
                                          const glm::vec3& halfExtents2, glm::vec3& penetrationDepth) {
  // Calculate the distance between centers
  glm::vec3 distance = pos1 - pos2;

  // Calculate the sum of half extents
  glm::vec3 totalHalfExtents = halfExtents1 + halfExtents2;

  // Check for overlap on all axes
  glm::vec3 overlap = totalHalfExtents - glm::abs(distance);

  // If any axis has no overlap, there's no collision
  if (overlap.x <= 0.0f || overlap.y <= 0.0f || overlap.z <= 0.0f) {
    return false;
  }

  // Find the axis with minimum overlap (this is our separation axis)
  if (overlap.x <= overlap.y && overlap.x <= overlap.z) {
    penetrationDepth = glm::vec3(overlap.x * (distance.x < 0 ? -1.0f : 1.0f), 0.0f, 0.0f);
  } else if (overlap.y <= overlap.z) {
    penetrationDepth = glm::vec3(0.0f, overlap.y * (distance.y < 0 ? -1.0f : 1.0f), 0.0f);
  } else {
    penetrationDepth = glm::vec3(0.0f, 0.0f, overlap.z * (distance.z < 0 ? -1.0f : 1.0f));
  }

  return true;
}

void plugins::Physics::build(Game& game) {
  game.addSystem(Schedule::Update, [](std::shared_ptr<entt::registry>& registry, resources::Time& time) {
    float deltaTime = time.deltaTime;

    // Update velocities first
    registry->view<components::AngularVelocity, components::AngularAcceleration>().each(
        [&registry, deltaTime](entt::entity entity, auto& angularVelocity, const auto& angularAcceleration) {
          angularVelocity.value += angularAcceleration.value * deltaTime;
          registry->replace<components::AngularVelocity>(entity, angularVelocity);
        });

    registry->view<components::Velocity, components::Acceleration>().each(
        [&registry, deltaTime](entt::entity entity, auto& velocity, const auto& acceleration) {
          velocity.value += acceleration.value * deltaTime;
          registry->replace<components::Velocity>(entity, velocity);
        });

    // Store original positions for collision detection
    auto colliderView = registry->view<components::Position, components::BoxCollider>();
    std::vector<std::pair<entt::entity, glm::vec3>> originalPositions;

    for (auto entity : colliderView) {
      auto& pos = colliderView.get<components::Position>(entity);
      originalPositions.emplace_back(entity, pos.value);
    }

    // Update rotation based on angular velocity
    registry->view<components::Rotation, components::AngularVelocity>().each(
        [&registry, deltaTime](entt::entity entity, auto& rotation, const auto& angularVelocity) {
          glm::vec3 angularVel = angularVelocity.value;
          float angle = glm::length(angularVel) * deltaTime;

          if (angle > 0.0f) {
            glm::vec3 axis = glm::normalize(angularVel);
            glm::quat deltaRotation = glm::angleAxis(angle, axis);
            rotation.value = deltaRotation * rotation.value;
            registry->replace<components::Rotation>(entity, rotation);
          }
        });

    // Update positions based on velocity
    registry->view<components::Position, components::Velocity>().each(
        [&registry, deltaTime](entt::entity entity, auto& position, const auto& velocity) {
          position.value += velocity.value * deltaTime;
          registry->replace<components::Position>(entity, position);
        });

    // Collision detection and response
    std::vector<plugins::physics::components::CollisionEvent> collisionEvents;

    // Check all pairs of colliders
    for (auto entity1 : colliderView) {
      auto& pos1 = colliderView.get<components::Position>(entity1);
      auto& collider1 = colliderView.get<components::BoxCollider>(entity1);

      for (auto entity2 : colliderView) {
        // Don't check collision with self
        if (entity1 >= entity2)
          continue;

        auto& pos2 = colliderView.get<components::Position>(entity2);
        auto& collider2 = colliderView.get<components::BoxCollider>(entity2);

        glm::vec3 penetrationDepth;
        if (checkAABBCollision(pos1.value, collider1.halfExtents, pos2.value, collider2.halfExtents, penetrationDepth)) {
          // Create collision event
          plugins::physics::components::CollisionEvent event;
          event.entity1 = entity1;
          event.entity2 = entity2;
          event.penetrationDepth = penetrationDepth;
          collisionEvents.push_back(event);

          // Improved collision response
          glm::vec3 collisionNormal = glm::normalize(penetrationDepth);
          float penetrationMagnitude = glm::length(penetrationDepth);

          // Separate objects by full penetration distance plus small margin
          glm::vec3 separation = collisionNormal * (penetrationMagnitude + 0.001f);

          // Check if entities have velocity to determine mass-like behavior
          auto* vel1 = registry->try_get<components::Velocity>(entity1);
          auto* vel2 = registry->try_get<components::Velocity>(entity2);

          if (vel1 && vel2) {
            // Both objects can move - split separation
            pos1.value += separation * 0.5f;
            pos2.value -= separation * 0.5f;

            // Apply velocity changes for bouncing/energy loss
            float restitution = 0.3f; // Energy loss factor
            glm::vec3 relativeVelocity = vel1->value - vel2->value;
            float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

            if (velocityAlongNormal > 0) {
              continue; // Objects separating
            }

            float impulse = -(1 + restitution) * velocityAlongNormal;
            glm::vec3 impulseVector = impulse * collisionNormal;

            vel1->value += impulseVector * 0.5f;
            vel2->value -= impulseVector * 0.5f;

            registry->replace<components::Velocity>(entity1, *vel1);
            registry->replace<components::Velocity>(entity2, *vel2);
          } else if (vel1) {
            // Only entity1 can move (entity2 is static)
            pos1.value += separation;
            if (glm::dot(vel1->value, collisionNormal) < 0) {
              // Remove velocity component in collision direction
              vel1->value -= glm::dot(vel1->value, collisionNormal) * collisionNormal * 1.3f;
              registry->replace<components::Velocity>(entity1, *vel1);
            }
          } else if (vel2) {
            // Only entity2 can move (entity1 is static)
            pos2.value -= separation;
            if (glm::dot(vel2->value, collisionNormal) > 0) {
              // Remove velocity component in collision direction
              vel2->value -= glm::dot(vel2->value, collisionNormal) * collisionNormal * 1.3f;
              registry->replace<components::Velocity>(entity2, *vel2);
            }
          } else {
            // Both are static - just separate
            pos1.value += separation * 0.5f;
            pos2.value -= separation * 0.5f;
          }

          // Update positions in registry
          registry->replace<components::Position>(entity1, pos1);
          registry->replace<components::Position>(entity2, pos2);
        }
      }
    }

    // Process collision callbacks
    for (const auto& event : collisionEvents) {
      // Check if entities have collision callbacks
      auto* callback1 = registry->try_get<plugins::physics::components::CollisionCallback>(event.entity1);
      auto* callback2 = registry->try_get<plugins::physics::components::CollisionCallback>(event.entity2);

      if (callback1 && callback1->onCollisionEnter) {
        callback1->onCollisionEnter(event);
      }
      if (callback2 && callback2->onCollisionEnter) {
        // Create event from entity2's perspective
        plugins::physics::components::CollisionEvent reverseEvent = event;
        reverseEvent.entity1 = event.entity2;
        reverseEvent.entity2 = event.entity1;
        reverseEvent.penetrationDepth = -event.penetrationDepth;
        callback2->onCollisionEnter(reverseEvent);
      }
    }
  });
}
