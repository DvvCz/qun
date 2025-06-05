#include "game.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <string>

#include "input/raw/keyboard.hpp"
#include "input/raw/mouse.hpp"

Game::Game() {
  registry = std::make_shared<entt::registry>();
}

void Game::addDefaultSystems() {
  // Rendering system
  addSystem(Schedule::Startup, [this]() -> std::expected<void, std::string> {
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

    return {};
  });
  addSystem<entt::registry>(Schedule::Render, [this](entt::registry& reg) {
    renderer->drawFrame();
    glfwSwapBuffers(window->getGlfwWindow());
  });

  // Time updating system
  addSystem<resources::Time>(Schedule::Startup, [this](resources::Time& time) {
    time.currentTime = glfwGetTime();
    time.lastTime = time.currentTime;
    time.deltaTime = 0.0f;
  });
  addSystem<resources::Time>(Schedule::Update, [](resources::Time& time) {
    time.currentTime = glfwGetTime();
    time.deltaTime = time.currentTime - time.lastTime;
    time.lastTime = time.currentTime;
  });

  // Camera controller
  struct CameraState {
    float yaw = 0.0f;
    float pitch = 0.0f;
    float mouseSensitivity = 0.002f;
    float cameraSpeed = 3.0f;
  };
  auto cameraState = std::make_shared<CameraState>();

  addSystem(Schedule::Startup, [this]() {
    // Hide cursor for first-person camera
    glfwSetInputMode(window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    renderer->setCameraPos(glm::vec3(-5.0f, 0.0f, 0.5f));
  });

  addSystem<resources::Time>(Schedule::Update, [cameraState, this](resources::Time& time) {
    auto mouseDelta = input::Mouse::getPositionDelta();
    cameraState->yaw -= mouseDelta.x * cameraState->mouseSensitivity;
    cameraState->pitch -= mouseDelta.y * cameraState->mouseSensitivity; // Invert Y axis for natural camera movement

    // Clamp pitch to prevent camera flipping
    cameraState->pitch = std::clamp(cameraState->pitch, -1.5f, 1.5f); // About ±85 degrees in radians

    // Calculate camera direction from yaw and pitch
    glm::vec3 cameraFront;
    cameraFront.x = cos(cameraState->yaw) * cos(cameraState->pitch);
    cameraFront.y = sin(cameraState->yaw) * cos(cameraState->pitch);
    cameraFront.z = sin(cameraState->pitch);
    cameraFront = glm::normalize(cameraFront);

    // Calculate right and up vectors for movement
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0f, 0.0f, 1.0f)));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

    // Movement relative to camera direction
    glm::vec3 cameraPos = renderer->getCameraPos();
    float velocity = cameraState->cameraSpeed * time.deltaTime;

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
  });
}

std::expected<bool, std::string> Game::start() {
  auto startupResult = runSchedule(Schedule::Startup);
  if (!startupResult) {
    return std::unexpected(startupResult.error());
  }

  while (!window->shouldClose()) {
    // Update
    {
      glfwPollEvents();

      // Needs to run after glfwPollEvents
      if (input::Keyboard::wasJustPressed(input::Key::Escape)) {
        break;
      }

      auto updateResult = runSchedule(Schedule::Update);
      if (!updateResult) {
        return std::unexpected(updateResult.error());
      }

      input::Keyboard::resetCurrentKeyMaps();
      input::Mouse::resetCurrentMouseMaps();
    }

    auto renderResult = runSchedule(Schedule::Render);
    if (!renderResult) {
      return std::unexpected(renderResult.error());
    }
  }

  glfwTerminate();

  return true;
}
