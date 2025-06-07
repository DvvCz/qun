#include "transform.hpp"

#include "components/transform.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <print>

#include "game.hpp"

// TODO: when parent relationships are implemented, this is gonna be a lot more complex
// TODO: Make these individual so you can still add position/rotation/scale at once.

static void updateGlobalTransformEmplace(entt::registry& registry, entt::entity entity) {
  auto position = registry.get_or_emplace<components::Position>(entity, glm::vec3(0.0f));
  auto rotation = registry.get_or_emplace<components::Rotation>(entity, glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
  auto scale = registry.get_or_emplace<components::Scale>(entity, glm::vec3(1.0f));

  glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position.value);
  glm::mat4 rotationMatrix = glm::mat4_cast(rotation.value);
  glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale.value);

  glm::mat4 globalTransform = translationMatrix * rotationMatrix * scaleMatrix;
  registry.emplace_or_replace<components::GlobalTransform>(entity, globalTransform);
}

static void updateGlobalTransformOptional(entt::registry& registry, entt::entity entity) {
  auto position = registry.try_get<components::Position>(entity);
  auto rotation = registry.try_get<components::Rotation>(entity);
  auto scale = registry.try_get<components::Scale>(entity);

  glm::vec3 positionValue = position ? position->value : glm::vec3(0.0f);
  glm::quat rotationValue = rotation ? rotation->value : glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  glm::vec3 scaleValue = scale ? scale->value : glm::vec3(1.0f);

  glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), positionValue);
  glm::mat4 rotationMatrix = glm::mat4_cast(rotationValue);
  glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scaleValue);

  glm::mat4 globalTransform = translationMatrix * rotationMatrix * scaleMatrix;
  registry.emplace_or_replace<components::GlobalTransform>(entity, globalTransform);
}

static void startup(std::shared_ptr<entt::registry> registry) {
  registry->on_update<components::Position>().connect<updateGlobalTransformEmplace>();
  registry->on_construct<components::Position>().connect<updateGlobalTransformOptional>();

  registry->on_update<components::Rotation>().connect<updateGlobalTransformEmplace>();
  registry->on_construct<components::Rotation>().connect<updateGlobalTransformOptional>();

  registry->on_update<components::Scale>().connect<updateGlobalTransformEmplace>();
  registry->on_construct<components::Scale>().connect<updateGlobalTransformOptional>();
}

void plugins::Transform::build(Game& game) {
  game.addSystem(Schedule::Startup, startup);
}
