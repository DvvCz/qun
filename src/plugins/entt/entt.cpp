#include "entt.hpp"

#include "game.hpp"

#include <entt/entt.hpp>

void plugins::EnTT::build(Game& game) {
  std::shared_ptr<entt::registry> registry;

  game.addResource(registry);

  /* clang-format off */
  game.addSystem(Schedule::Startup, [](std::shared_ptr<entt::registry>& registry){
    registry = std::make_shared<entt::registry>();
  }); /* clang-format on */
}
