#include "game.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <string>
#include <memory>

#include "input/raw/keyboard.hpp"
#include "input/raw/mouse.hpp"

#include "resources/time.hpp"

#include "render/window.hpp"
#include "render/renderer.hpp"

std::expected<bool, std::string> Game::start() {
  isRunning = true;

  auto startupResult = runSchedule(Schedule::Startup);
  if (!startupResult) {
    return std::unexpected(startupResult.error());
  }

  while (isRunning) {
    auto updateResult = runSchedule(Schedule::Update);
    if (!updateResult) {
      return std::unexpected(updateResult.error());
    }

    auto postUpdateResult = runSchedule(Schedule::PostUpdate);
    if (!postUpdateResult) {
      return std::unexpected(postUpdateResult.error());
    }

    auto renderResult = runSchedule(Schedule::Render);
    if (!renderResult) {
      return std::unexpected(renderResult.error());
    }
  }

  glfwTerminate();

  return true;
}

void Game::requestExit() {
  isRunning = false;
}
