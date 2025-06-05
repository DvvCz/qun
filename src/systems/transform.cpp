#include "transform.hpp"

#include "components/transform.hpp"
#include <glm/ext/matrix_transform.hpp>

// TODO: when parent relationships are implemented, this is gonna be a lot more complex

static void on_position_update(entt::registry& registry, entt::entity entity) {
  auto globalTransform = registry.get_or_emplace<components::GlobalTransform>(entity, glm::identity<glm::mat4x4>());
  auto position = registry.get<components::Position>(entity);

  globalTransform[3] = glm::vec4(position.x, position.y, position.z, 1.0f);
  registry.emplace_or_replace<components::GlobalTransform>(entity, globalTransform);
}

static void on_position_construct(entt::registry& registry, entt::entity entity) {
  auto globalTransform = registry.get_or_emplace<components::GlobalTransform>(entity, glm::identity<glm::mat4x4>());
  auto position = registry.get<components::Position>(entity);

  globalTransform[3] = glm::vec4(position.x, position.y, position.z, 1.0f);
  registry.emplace_or_replace<components::GlobalTransform>(entity, globalTransform);
}

void systems::transform::startup(entt::registry& registry) {
  registry.on_update<components::Position>().connect<on_position_update>();
  registry.on_construct<components::Position>().connect<on_position_construct>();
}
