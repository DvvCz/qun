#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <print>
#include <fstream>

#include "input/raw/keyboard.hpp"
#include "input/raw/mouse.hpp"
#include "render/renderer.hpp"
#include "render/window.hpp"

#include "resource/obj/obj.hpp"
#include "resource/img/img.hpp"

int main() {
  if (!glfwInit()) {
    std::println("Failed to initialize GLFW");
    return EXIT_FAILURE;
  }

  auto window = std::make_shared<Window>(800, 600, "OpenGL Window");
  Input::Keyboard::bindGlfwCallbacks(window->getGlfwWindow());

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::println("Failed to initialize GLAD");
    return EXIT_FAILURE;
  }

  auto renderer = std::make_unique<Renderer>(window);

  auto out = resource::ObjAsset::tryFromFile("../resources/78717.obj");
  if (out.has_value()) {
    std::println("Successfully loaded OBJ file");
    renderer->addModel(out.value());
  } else {
    std::println("Failed to load OBJ file: {}", out.error());
    return EXIT_FAILURE;
  }

  while (!window->shouldClose()) {
    // Update
    {
      glfwPollEvents();

      // Needs to run after glfwPollEvents
      if (Input::Keyboard::wasJustPressed(Input::Key::Escape)) {
        break;
      }

      Input::Keyboard::resetCurrentKeyMaps();
    }

    // Draw
    {
      renderer->drawFrame();
      glfwSwapBuffers(window->getGlfwWindow());
    }
  }

  glfwTerminate();

  return 0;
}
