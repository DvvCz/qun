#include "transform.hpp"

#include "components/transform.hpp"
#include <print>

systems::Transform::Transform(const std::shared_ptr<entt::registry>& registry) : registry(registry) {
  registry->on_update<components::Position>().connect<&systems::Transform::update>();
  registry->on_construct<components::Position>().connect<&systems::Transform::construct>();
}

void systems::Transform::update(entt::registry& reg, entt::entity entity) {
  std::println("Updated position component");
}

void systems::Transform::construct(entt::registry& registry, entt::entity entity) {
}
