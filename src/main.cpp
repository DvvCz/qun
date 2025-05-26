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
#include "render/model/3d/cube.hpp"
#include "render/model/2d/quad.hpp"
#include "render/material/material2d.hpp"
#include "render/material/material3d.hpp"
#include "render/vertex.hpp"

#include "components/model.hpp"
#include "components/transform.hpp"
#include "components/light.hpp"
#include "components/material.hpp"

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

  auto redMaterial = std::make_shared<material::Block3D>();
  redMaterial->ambient = glm::vec3(0.2f, 0.05f, 0.05f);
  redMaterial->diffuse = glm::vec3(0.8f, 0.2f, 0.2f);
  redMaterial->specular = glm::vec3(1.0f, 1.0f, 1.0f);
  redMaterial->shininess = 64.0f;
  redMaterial->dissolve = 1.0f;

  // Create a shiny material for the bunny
  auto bunnyMaterial = std::make_shared<material::Block3D>();
  bunnyMaterial->ambient = glm::vec3(0.2f, 0.2f, 0.2f);
  bunnyMaterial->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
  bunnyMaterial->specular = glm::vec3(1.0f, 1.0f, 1.0f);
  bunnyMaterial->shininess = 32.0f;
  bunnyMaterial->dissolve = 1.0f;

  auto out = resource::ObjAsset::tryFromFile("resources/bunny.obj");
  if (out.has_value()) {
    auto assetModel = renderer->createAsset3D(out.value());

    auto modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, -0.8f, 0.0f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, modelMatrix);
    registry->emplace<components::Model3D>(ent, assetModel);
    registry->emplace<components::Material3D>(ent, bunnyMaterial);
  }

  auto mainLight = registry->create();
  auto mainLightMatrix = glm::mat4(1.0f);
  mainLightMatrix = glm::translate(mainLightMatrix, glm::vec3(0.0f, 0.0f, 5.0f));
  registry->emplace<components::Light>(mainLight, glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, 1000.0f);
  registry->emplace<components::GlobalTransform>(mainLight, mainLightMatrix);

  auto topCubeModel =
      std::make_shared<model::Cube>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::quat(glm::vec3(0.0f)));
  auto topCubeEnt = registry->create();
  auto topCubeMatrix = glm::mat4(1.0f);
  topCubeMatrix = glm::rotate(topCubeMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  registry->emplace<components::GlobalTransform>(topCubeEnt, topCubeMatrix);
  registry->emplace<components::Model3D>(topCubeEnt, topCubeModel);
  registry->emplace<components::Material3D>(topCubeEnt, redMaterial);

  // add a cube to draw
  auto baseModel = std::make_shared<model::Cube>(glm::vec3(0.0f, 0.0f, -0.5f), glm::vec3(1000.0f, 1000.0f, 0.1f),
                                                 glm::quat(glm::vec3(0.0f)));
  auto baseEnt = registry->create();
  registry->emplace<components::GlobalTransform>(baseEnt, glm::mat4(1.0f));
  registry->emplace<components::Model3D>(baseEnt, baseModel);
  registry->emplace<components::Material3D>(baseEnt, redMaterial);

  /* clang-format off */
  auto basequad = std::make_shared<model::Quad>(
      Vertex2D{glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
      Vertex2D{glm::vec3(1.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
      Vertex2D{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
      Vertex2D{glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)}
  ); /* clang-format on */
  auto quadEnt = registry->create();
  registry->emplace<components::GlobalTransform>(quadEnt, glm::mat4(1.0f));
  registry->emplace<components::Model2D>(quadEnt, basequad);
  registry->emplace<components::Material2D>(quadEnt, glm::vec3(0.0f, 0.0f, 1.0f));

  float deltaTime = 0.0f;
  float lastTime = glfwGetTime();

  float yaw = 0.0f;
  float pitch = 0.0f;
  float mouseSensitivity = 0.002f;
  float cameraSpeed = 3.0f;

  // Hide cursor for first-person camera
  glfwSetInputMode(window->getGlfwWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
