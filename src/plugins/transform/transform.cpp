#include "transform.hpp"

#include "components/transform.hpp"
#include "components/parent.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <print>

#include "game.hpp"

// TODO: when parent relationships are implemented, this is gonna be a lot more complex
// TODO: Make these individual so you can still add position/rotation/scale at once.

// Helper function to compute local transform matrix
static glm::mat4 computeLocalTransform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale) {
  glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position);
  glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
  glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
  return translationMatrix * rotationMatrix * scaleMatrix;
}

// Helper function to get parent's global transform
static glm::mat4 getParentGlobalTransform(entt::registry& registry, entt::entity entity) {
  auto* child = registry.try_get<components::Child>(entity);
  if (!child) {
    return glm::identity<glm::mat4x4>();
  }

  // Check if parent entity is valid
  if (!registry.valid(child->parent)) {
    return glm::identity<glm::mat4x4>();
  }

  auto* parentGlobalTransform = registry.try_get<components::GlobalTransform>(child->parent);
  if (!parentGlobalTransform) {
    return glm::identity<glm::mat4x4>();
  }

  return parentGlobalTransform->value;
}

// Recursively update an entity and all its children
static void updateEntityAndChildren(entt::registry& registry, entt::entity entity) {
  // Safety check: ensure entity is valid
  if (!registry.valid(entity)) {
    return;
  }

  // Get local transform components
  auto position = registry.get_or_emplace<components::Position>(entity, glm::vec3(0.0f));
  auto rotation = registry.get_or_emplace<components::Rotation>(entity, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  auto scale = registry.get_or_emplace<components::Scale>(entity, glm::vec3(1.0f));

  // Compute local transform
  glm::mat4 localTransform = computeLocalTransform(position.value, rotation.value, scale.value);

  // Get parent's global transform
  glm::mat4 parentGlobalTransform = getParentGlobalTransform(registry, entity);

  // Compute and set global transform
  glm::mat4 globalTransform = parentGlobalTransform * localTransform;
  registry.emplace_or_replace<components::GlobalTransform>(entity, globalTransform);

  // Update all children
  auto* parent = registry.try_get<components::Parent>(entity);
  if (parent) {
    for (entt::entity child : parent->children) {
      // Safety check: ensure child is valid and not the same as current entity (prevent cycles)
      if (registry.valid(child) && child != entity) {
        updateEntityAndChildren(registry, child);
      }
    }
  }
}

// Recursively update an entity and all its children (optional version for construction)
static void updateEntityAndChildrenOptional(entt::registry& registry, entt::entity entity) {
  // Safety check: ensure entity is valid
  if (!registry.valid(entity)) {
    return;
  }

  // Get local transform components (optional)
  auto position = registry.try_get<components::Position>(entity);
  auto rotation = registry.try_get<components::Rotation>(entity);
  auto scale = registry.try_get<components::Scale>(entity);

  glm::vec3 positionValue = position ? position->value : glm::vec3(0.0f);
  glm::quat rotationValue = rotation ? rotation->value : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 scaleValue = scale ? scale->value : glm::vec3(1.0f);

  // Compute local transform
  glm::mat4 localTransform = computeLocalTransform(positionValue, rotationValue, scaleValue);

  // Get parent's global transform
  glm::mat4 parentGlobalTransform = getParentGlobalTransform(registry, entity);

  // Compute and set global transform
  glm::mat4 globalTransform = parentGlobalTransform * localTransform;
  registry.emplace_or_replace<components::GlobalTransform>(entity, globalTransform);

  // Update all children
  auto* parent = registry.try_get<components::Parent>(entity);
  if (parent) {
    for (entt::entity child : parent->children) {
      // Safety check: ensure child is valid and not the same as current entity (prevent cycles)
      if (registry.valid(child) && child != entity) {
        updateEntityAndChildrenOptional(registry, child);
      }
    }
  }
}

static void startup(std::shared_ptr<entt::registry> registry) {
  // Handle transform component updates
  registry->on_update<components::Position>().connect<updateEntityAndChildren>();
  registry->on_construct<components::Position>().connect<updateEntityAndChildrenOptional>();

  registry->on_update<components::Rotation>().connect<updateEntityAndChildren>();
  registry->on_construct<components::Rotation>().connect<updateEntityAndChildrenOptional>();

  registry->on_update<components::Scale>().connect<updateEntityAndChildren>();
  registry->on_construct<components::Scale>().connect<updateEntityAndChildrenOptional>();

  // Parent relationships
  registry->on_update<components::Parent>().connect<updateEntityAndChildren>();
  registry->on_construct<components::Parent>().connect<updateEntityAndChildrenOptional>();

  registry->on_construct<components::Child>().connect<updateEntityAndChildrenOptional>();
  registry->on_update<components::Child>().connect<updateEntityAndChildren>();
}

void plugins::Transform::build(Game& game) {
  game.addSystem(Schedule::Startup, startup);
}
