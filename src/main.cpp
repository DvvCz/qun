#include "game.hpp"

#include <print>

#include "util/error.hpp"

#include "scenes/nfs.hpp"

int main() {
  auto game = std::make_unique<Game>();
  game->addDefaultSystems();
  game->addSystem<entt::registry, Renderer>(Schedule::Startup, scenes::nfs::startup);
  game->addSystem<entt::registry, Renderer>(Schedule::Update, scenes::nfs::update);

  auto result = game->start();
  if (!result.has_value()) {
    std::println(stderr, "Failed to start game:\n\t{}", util::error::indent(result.error()));
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
