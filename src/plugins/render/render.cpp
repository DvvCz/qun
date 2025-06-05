#include "render.hpp"

#include "game.hpp"
#include "render/renderer.hpp"
#include "render/window.hpp"

#include "input/raw/keyboard.hpp"
#include "input/raw/mouse.hpp"

void plugins::Render::build(Game& game) {
  std::shared_ptr<Window> window;
  std::shared_ptr<Renderer> renderer;

  game.addResource(window);
  game.addResource(renderer);

  /* clang-format off */
  game.addSystem(Schedule::Startup, [](std::shared_ptr<entt::registry>& registry, std::shared_ptr<Window>& window, std::shared_ptr<Renderer>& renderer) -> std::expected<void, std::string> {
    if (!glfwInit()) {
      return std::unexpected("Failed to initialize GLFW");
    }

    window = std::make_shared<Window>(1280, 720, "OpenGL Window");
    input::Keyboard::bindGlfwCallbacks(window->getGlfwWindow());
    input::Mouse::bindGlfwCallbacks(window->getGlfwWindow());

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      return std::unexpected("Failed to initialize GLAD");
    }

    renderer = std::make_shared<Renderer>(window, registry);

    return {};
  }); /* clang-format on */

  game.addSystem(Schedule::Render, [](std::shared_ptr<Window>& window, std::shared_ptr<Renderer>& renderer) {
    renderer->drawFrame();
    glfwSwapBuffers(window->getGlfwWindow());
  });
}
