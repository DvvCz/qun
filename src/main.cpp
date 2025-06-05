#include "game.hpp"

#include <print>

#include "util/error.hpp"

#include "scenes/test.hpp"

int main() {
  auto game = std::make_unique<Game>();
  game->addDefaultSystems();
  game->addDefaultCameraController();
  game->addSystem<entt::registry, Renderer>(Schedule::Startup, scenes::test::startup);
  game->addSystem<entt::registry, Renderer>(Schedule::Update, scenes::test::update);

  auto result = game->start();
  if (!result.has_value()) {
    std::println(stderr, "Failed to start game:\n\t{}", util::error::indent(result.error()));
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
