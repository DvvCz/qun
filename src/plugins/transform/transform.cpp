#include "transform.hpp"

#include "components/transform.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <print>

#include "game.hpp"

// TODO: when parent relationships are implemented, this is gonna be a lot more complex
// TODO: Make these individual so you can still add position/rotation/scale at once.

static void update_global_transform(entt::registry& registry, entt::entity entity) {
  auto position = registry.get_or_emplace<components::Position>(entity, glm::vec3(0.0f));
  auto rotation = registry.get_or_emplace<components::Rotation>(entity, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  auto scale = registry.get_or_emplace<components::Scale>(entity, glm::vec3(1.0f));

  glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position.value);
  glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
  glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale.value);

  glm::mat4 globalTransform = translationMatrix * rotationMatrix * scaleMatrix;
  registry.emplace_or_replace<components::GlobalTransform>(entity, globalTransform);
}

static void startup(std::shared_ptr<entt::registry> registry) {
  registry->on_update<components::Position>().connect<update_global_transform>();
  registry->on_construct<components::Position>().connect<update_global_transform>();

  registry->on_update<components::Rotation>().connect<update_global_transform>();
  registry->on_construct<components::Rotation>().connect<update_global_transform>();

  registry->on_update<components::Scale>().connect<update_global_transform>();
  registry->on_construct<components::Scale>().connect<update_global_transform>();
}

void plugins::Transform::build(Game& game) {
  game.addSystem(Schedule::Startup, startup);
}
