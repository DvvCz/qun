#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <print>
#include <algorithm>
#include <entt/entt.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "input/raw/keyboard.hpp"
#include "input/raw/mouse.hpp"
#include "render/renderer.hpp"
#include "render/window.hpp"
#include "render/model/cube.hpp"

#include "components/model.hpp"
#include "components/transform.hpp"

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

  auto registry = std::make_shared<entt::registry>();
  auto renderer = std::make_unique<Renderer>(window, registry);

  auto redMaterial = std::make_shared<MaterialBlock>();
  redMaterial->ambient = glm::vec3(0.1f, 0.0f, 0.0f);
  redMaterial->diffuse = glm::vec3(0.7f, 0.7f, 0.7f);
  redMaterial->specular = glm::vec3(0.8f, 0.8f, 0.8f);
  redMaterial->shininess = 32.0f;
  redMaterial->dissolve = 1.0f;

  auto out = resource::ObjAsset::tryFromFile("../resources/bunnyNoNorm.obj");
  if (out.has_value()) {
    auto assetModel = renderer->createAssetModel(out.value());

    auto modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(10));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, modelMatrix);
    registry->emplace<components::Model>(ent, assetModel);
  }

  // add a cube to draw
  auto cubeModel =
      std::make_shared<CubeModel>(glm::vec3(0.0f, 0.0f, -0.5f), glm::vec3(1000.0f, 1000.0f, 0.1f), glm::quat(glm::vec3(0.0f)));
  auto cubeEnt = registry->create();
  registry->emplace<components::GlobalTransform>(cubeEnt, glm::mat4(1.0f));
  registry->emplace<components::Model>(cubeEnt, cubeModel);
  registry->emplace<components::Material>(cubeEnt, redMaterial);

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
      float shiftMul = Input::Keyboard::isCurrentlyHeld(Input::Key::LeftShift) ? 2.0f : 1.0f;

      if (Input::Keyboard::isCurrentlyHeld(Input::Key::W)) {
        cameraPos += cameraFront * velocity * shiftMul;
      }
      if (Input::Keyboard::isCurrentlyHeld(Input::Key::S)) {
        cameraPos -= cameraFront * velocity * shiftMul;
      }
      if (Input::Keyboard::isCurrentlyHeld(Input::Key::A)) {
        cameraPos -= cameraRight * velocity * shiftMul;
      }
      if (Input::Keyboard::isCurrentlyHeld(Input::Key::D)) {
        cameraPos += cameraRight * velocity * shiftMul;
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
