#include "game.hpp"

#include <print>

#include "util/error.hpp"

int main() {
  auto game = std::make_unique<Game>();

  game->addSystem<resources::Time>(Schedule::Startup, [](resources::Time& time) {
    // t
    std::println("Game startup: time is {}", time.currentTime);
  });

  auto result = game->start();
  if (!result.has_value()) {
    std::println(stderr, "Failed to start game:\n\t{}", util::error::indent(result.error()));
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
