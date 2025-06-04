#include "game.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <string>

#include "input/raw/keyboard.hpp"
#include "input/raw/mouse.hpp"

#include "systems/particle.hpp"

Game::Game() {
  registry = std::make_shared<entt::registry>();
  particleSystem = std::make_unique<systems::Particle>(registry);
  transformSystem = std::make_unique<systems::Transform>(registry);
}

std::expected<bool, std::string> Game::start() {
  if (!glfwInit()) {
    return std::unexpected("Failed to initialize GLFW");
  }

  window = std::make_shared<Window>(1280, 720, "OpenGL Window");
  input::Keyboard::bindGlfwCallbacks(window->getGlfwWindow());
  input::Mouse::bindGlfwCallbacks(window->getGlfwWindow());

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    return std::unexpected("Failed to initialize GLAD");
  }

  renderer = std::make_unique<Renderer>(window, registry);

  auto setupSceneResult = setupScene();
  if (!setupSceneResult.has_value()) {
    return std::unexpected{setupSceneResult.error()};
  }

  float deltaTime = 0.0f;
  float lastTime = glfwGetTime();

  float yaw = 0.0f;
  float pitch = 0.0f;
  float mouseSensitivity = 0.002f;
  float cameraSpeed = 3.0f;

  // Hide cursor for first-person camera
  glfwSetInputMode(window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  renderer->setCameraPos(glm::vec3(-5.0f, 0.0f, 0.5f));

  while (!window->shouldClose()) {
    // Update
    {
      glfwPollEvents();

      // Needs to run after glfwPollEvents
      if (input::Keyboard::wasJustPressed(input::Key::Escape)) {
        break;
      }

      float curTime = glfwGetTime();
      deltaTime = curTime - lastTime;
      lastTime = curTime;

      particleSystem->tick(curTime, deltaTime);

      // Mouse look
      auto mouseDelta = input::Mouse::getPositionDelta();
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

      float modifier = 1.0;
      if (input::Keyboard::isCurrentlyHeld(input::Key::LeftControl)) {
        modifier *= 0.25f;
      }
      if (input::Keyboard::isCurrentlyHeld(input::Key::LeftShift)) {
        modifier *= 2.0f;
      }

      if (input::Keyboard::isCurrentlyHeld(input::Key::W)) {
        cameraPos += cameraFront * velocity * modifier;
      }
      if (input::Keyboard::isCurrentlyHeld(input::Key::S)) {
        cameraPos -= cameraFront * velocity * modifier;
      }
      if (input::Keyboard::isCurrentlyHeld(input::Key::A)) {
        cameraPos -= cameraRight * velocity * modifier;
      }
      if (input::Keyboard::isCurrentlyHeld(input::Key::D)) {
        cameraPos += cameraRight * velocity * modifier;
      }

      // Update camera position and direction
      renderer->setCameraPos(cameraPos);
      renderer->setCameraDir(cameraFront);

      input::Keyboard::resetCurrentKeyMaps();
      input::Mouse::resetCurrentMouseMaps();
    }

    // Draw
    {
      renderer->drawFrame();
      glfwSwapBuffers(window->getGlfwWindow());
    }
  }

  glfwTerminate();

  return true;
}
