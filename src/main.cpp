#include "game.hpp"

#include <print>

int main() {
  auto game = Game();

  auto result = game.start();
  if (!result.has_value()) {
    std::println(stderr, "Failed to start game: {}", result.error());
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
