#include "input.hpp"

#include "game.hpp"

#include <entt/entt.hpp>

#include "input/raw/keyboard.hpp"
#include "input/raw/mouse.hpp"

#include "render/window.hpp"

void plugins::Input::build(Game& game) {
  /* clang-format off */
  game.addSystem(Schedule::Update, [&](std::shared_ptr<Window>& window){
    glfwPollEvents();

    if (window->shouldClose()) {
      game.requestExit();
    }

    if (input::Keyboard::wasJustPressed(input::Key::Escape)) {
      game.requestExit();
    }
  }); /* clang-format on */

  game.addSystem(Schedule::PostUpdate, []() {
    input::Keyboard::resetCurrentKeyMaps();
    input::Mouse::resetCurrentMouseMaps();
  });
}
