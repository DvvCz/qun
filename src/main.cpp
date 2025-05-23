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
  Input::Mouse::bindGlfwCallbacks(window->getGlfwWindow());

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::println("Failed to initialize GLAD");
    return EXIT_FAILURE;
  }

  auto renderer = std::make_unique<Renderer>(window);

  auto out = resource::ObjAsset::tryFromFile("../resources/bunnyNoNorm.obj");
  if (out.has_value()) {
    renderer->addModel(out.value());
  } else {
    std::println("Failed to load OBJ file: {}", out.error());
    return EXIT_FAILURE;
  }

  float deltaTime = 0.0f;
  float lastTime = glfwGetTime();

  while (!window->shouldClose()) {
    // Update
    {
      glfwPollEvents();

      // Needs to run after glfwPollEvents
      if (Input::Keyboard::wasJustPressed(Input::Key::Escape)) {
        break;
      }

      float curTime = glfwGetTime();
      deltaTime = curTime - lastTime;
      lastTime = curTime;

      if (Input::Keyboard::isCurrentlyHeld(Input::Key::W)) {
        renderer->setCameraPos(renderer->getCameraPos() + glm::vec3(0.0f, 0.0f, -1.0f) * deltaTime);
      }

      if (Input::Keyboard::isCurrentlyHeld(Input::Key::S)) {
        renderer->setCameraPos(renderer->getCameraPos() + glm::vec3(0.0f, 0.0f, 1.0f) * deltaTime);
      }

      if (Input::Keyboard::isCurrentlyHeld(Input::Key::A)) {
        renderer->setCameraPos(renderer->getCameraPos() + glm::vec3(-1.0f, 0.0f, 0.0f) * deltaTime);
      }

      if (Input::Keyboard::isCurrentlyHeld(Input::Key::D)) {
        renderer->setCameraPos(renderer->getCameraPos() + glm::vec3(1.0, 0.0f, 0.0f) * deltaTime);
      }

      // auto lookTarget = renderer->getCameraPos() + glm::vec3(sin(mouseDelta.x), cos(-mouseDelta.y), 0.0f);
      // renderer->setLookAt(lookTarget);

      Input::Keyboard::resetCurrentKeyMaps();
      Input::Mouse::resetCurrentMouseMaps();
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
