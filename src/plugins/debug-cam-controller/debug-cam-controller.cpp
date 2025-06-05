#include "debug-cam-controller.hpp"

#include "game.hpp"
#include "render/renderer.hpp"
#include "render/window.hpp"
#include "resources/time.hpp"

#include "input/raw/mouse.hpp"
#include "input/raw/keyboard.hpp"

struct CameraState {
  float yaw = 0.0f;
  float pitch = 0.0f;
  float mouseSensitivity = 0.002f;
  float cameraSpeed = 3.0f;
};

void plugins::DebugCamController::build(Game& game) {
  auto cameraState = std::make_shared<CameraState>();

  game.addResource(cameraState);

  game.addSystem(Schedule::Startup, [](std::shared_ptr<Window> window, std::shared_ptr<Renderer> renderer) {
    // Hide cursor for first-person camera
    glfwSetInputMode(window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    renderer->setCameraPos(glm::vec3(-5.0f, 0.0f, 0.5f));
  });

  /* clang-format off */
  game.addSystem(Schedule::Update, [](
    resources::Time& time,
    std::shared_ptr<Renderer> renderer,
    std::shared_ptr<CameraState> cameraState
  ) {
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

    renderer->setCameraPos(cameraPos);
    renderer->setCameraDir(cameraFront);
  }); /* clang-format on */
}
