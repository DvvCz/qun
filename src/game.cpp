#include "game.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <print>
#include <string>

#include "components/light.hpp"
#include "components/material.hpp"
#include "components/model.hpp"
#include "components/transform.hpp"

#include "asset/obj/obj.hpp"
#include "asset/gltf/gltf.hpp"

#include "input/raw/keyboard.hpp"
#include "input/raw/mouse.hpp"
#include "render/model/3d/cube.hpp"

#include "util/error.hpp"

Game::Game() {
  registry = std::make_shared<entt::registry>();
}

std::expected<bool, std::string> Game::setupScene() {
  { // gltf fish
    auto asset = asset::loader::Gltf::tryFromFile("resources/BarramundiFish.glb", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load GLTF asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::scale(matrix, glm::vec3(5.0f));
    matrix = glm::translate(matrix, glm::vec3(0.0f, -1.0f, 1.0f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
  }

  { // main light
    auto matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, 5.0f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Light>(ent, glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, 1000.0f);
  }

  auto redMaterial = std::make_shared<material::Block3D>();
  redMaterial->ambient = glm::vec3(0.2f, 0.05f, 0.05f);
  redMaterial->diffuse = glm::vec3(0.8f, 0.2f, 0.2f);
  redMaterial->specular = glm::vec3(1.0f, 1.0f, 1.0f);
  redMaterial->shininess = 64.0f;
  redMaterial->dissolve = 1.0f;
  redMaterial->diffuseTextureId = -1;

  { // bunny
    auto asset = asset::loader::Obj::tryFromFile("resources/bunny.obj", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load bunny asset: {}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(0.0f, 2.0f, -0.325f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
  }

  { // red cube
    auto model = std::make_shared<model::Cube>(glm::vec3(1.0f));

    auto matrix = glm::mat4(1.0f);
    matrix = glm::rotate(matrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    matrix = glm::translate(matrix, glm::vec3(0.0f, 0.0f, 0.5f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
    registry->emplace<components::Material3D>(ent, redMaterial);
  }

  { // textured obj cube
    auto asset = asset::loader::Obj::tryFromFile("resources/cube-tex.obj", *renderer->textureManager3D);
    if (!asset.has_value()) {
      return std::unexpected{std::format("Failed to load textured cube asset:\n\t{}", util::error::indent(asset.error()))};
    }

    auto model = renderer->createAsset3D(asset.value());

    auto matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, glm::vec3(0.0f, -2.0f, 0.5f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
  }

  { // baseplate
    auto model = std::make_shared<model::Cube>(glm::vec3(1000.0f, 1000.0f, 0.01f));

    auto matrix = glm::mat4(1.0f);

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, matrix);
    registry->emplace<components::Model3D>(ent, model);
    registry->emplace<components::Material3D>(ent, redMaterial);
  }

  // /* clang-format off */
  // auto basequad = std::make_shared<model::Quad>(
  //     Vertex2D{glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 0.0f)},
  //     Vertex2D{glm::vec3(0.0f, -0.5f, 0.0f), glm::vec2(1.0f, 0.0f)},
  //     Vertex2D{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
  //     Vertex2D{glm::vec3(-0.5f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)}
  // ); /* clang-format on */
  // auto quadEnt = registry->create();
  // registry->emplace<components::GlobalTransform>(quadEnt, glm::mat4(1.0f));
  // registry->emplace<components::Model2D>(quadEnt, basequad);

  // auto blueMaterial = std::make_shared<material::Block2D>();
  // blueMaterial->color = glm::vec3(0.0f, 0.0f, 1.0f);
  // registry->emplace<components::Material2D>(quadEnt, blueMaterial);

  return true;
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
      float shiftMul = input::Keyboard::isCurrentlyHeld(input::Key::LeftShift) ? 2.0f : 1.0f;

      if (input::Keyboard::isCurrentlyHeld(input::Key::W)) {
        cameraPos += cameraFront * velocity * shiftMul;
      }
      if (input::Keyboard::isCurrentlyHeld(input::Key::S)) {
        cameraPos -= cameraFront * velocity * shiftMul;
      }
      if (input::Keyboard::isCurrentlyHeld(input::Key::A)) {
        cameraPos -= cameraRight * velocity * shiftMul;
      }
      if (input::Keyboard::isCurrentlyHeld(input::Key::D)) {
        cameraPos += cameraRight * velocity * shiftMul;
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
