#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <print>
#include <fstream>
#include <algorithm>

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

  auto window = std::make_shared<Window>(1280, 720, "OpenGL Window");
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

  // Camera control variables
  float yaw = 0.0f;                // Horizontal rotation
  float pitch = 0.0f;              // Vertical rotation
  float mouseSensitivity = 0.002f; // Mouse sensitivity
  float cameraSpeed = 3.0f;        // Movement speed

  // Hide cursor for first-person camera
  glfwSetInputMode(window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Initialize camera position to a reasonable distance from the bunny
  renderer->setCameraPos(glm::vec3(-5.0f, 0.0f, 0.0f));

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

      // Mouse look
      auto mouseDelta = Input::Mouse::getPositionDelta();
      yaw -= mouseDelta.x * mouseSensitivity;
      pitch -= mouseDelta.y * mouseSensitivity; // Invert Y axis for natural camera movement

      // Clamp pitch to prevent camera flipping
      pitch = std::clamp(pitch, -1.5f, 1.5f); // About ±85 degrees in radians

      // Calculate camera direction from yaw and pitch
      glm::vec3 cameraFront;
      cameraFront.x = cos(yaw) * cos(pitch);
      cameraFront.y = sin(yaw) * cos(pitch);
      cameraFront.z = sin(pitch);
      cameraFront = glm::normalize(cameraFront);

      // Calculate right and up vectors for movement
      glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 0.0f, 1.0f)));
      glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

      // Movement relative to camera direction
      glm::vec3 cameraPos = renderer->getCameraPos();
      float velocity = cameraSpeed * deltaTime;

      if (Input::Keyboard::isCurrentlyHeld(Input::Key::W)) {
        cameraPos += cameraFront * velocity;
      }
      if (Input::Keyboard::isCurrentlyHeld(Input::Key::S)) {
        cameraPos -= cameraFront * velocity;
      }
      if (Input::Keyboard::isCurrentlyHeld(Input::Key::A)) {
        cameraPos -= cameraRight * velocity;
      }
      if (Input::Keyboard::isCurrentlyHeld(Input::Key::D)) {
        cameraPos += cameraRight * velocity;
      }

      // Update camera position and direction
      renderer->setCameraPos(cameraPos);
      renderer->setCameraDir(cameraFront);

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
