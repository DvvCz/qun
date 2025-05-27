#include "game.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <print>
#include <string>

#include "components/light.hpp"
#include "components/material.hpp"
#include "components/model.hpp"
#include "components/transform.hpp"

#include "asset/img/img.hpp"
#include "asset/obj/obj.hpp"
#include "asset/gltf/gltf.hpp"

#include "input/raw/keyboard.hpp"
#include "input/raw/mouse.hpp"
#include "render/model/3d/cube.hpp"

Game::Game() {
  registry = std::make_shared<entt::registry>();
}

std::expected<bool, std::string> Game::setupScene() {
  auto boxAsset = asset::loader::Gltf::tryFromFile("resources/BarramundiFish.glb", *renderer->textureManager3D);
  if (!boxAsset.has_value()) {
    return std::unexpected{std::format("Failed to load GLTF asset: {}", boxAsset.error())};
  }

  std::println("Loaded GLTF asset with {} vertices and {} shapes", boxAsset.value().vertices.size(),
               boxAsset.value().shapes.size());

  auto boxModel = renderer->createAsset3D(boxAsset.value());
  auto boxEnt = registry->create();
  auto boxMatrix = glm::mat4(1.0f);
  boxMatrix = glm::scale(boxMatrix, glm::vec3(5.0f));
  boxMatrix = glm::translate(boxMatrix, glm::vec3(0.0f, -1.0f, 1.0f));
  registry->emplace<components::GlobalTransform>(boxEnt, boxMatrix);
  registry->emplace<components::Model3D>(boxEnt, boxModel);

  auto plateTexture = asset::loader::Img::tryFromFile("resources/NumernSchildAudiR8.png", *renderer->textureManager3D);
  if (!plateTexture.has_value()) {
    return std::unexpected{std::format("Failed to load texture: {}", plateTexture.error())};
  }

  auto redMaterial = std::make_shared<material::Block3D>();
  redMaterial->ambient = glm::vec3(0.2f, 0.05f, 0.05f);
  redMaterial->diffuse = glm::vec3(0.8f, 0.2f, 0.2f);
  redMaterial->specular = glm::vec3(1.0f, 1.0f, 1.0f);
  redMaterial->shininess = 64.0f;
  redMaterial->dissolve = 1.0f;
  redMaterial->diffuseTextureId = -1;

  // Create a shiny material for the bunny
  auto bunnyMaterial = std::make_shared<material::Block3D>();
  bunnyMaterial->ambient = glm::vec3(0.2f, 0.2f, 0.2f);
  bunnyMaterial->diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
  bunnyMaterial->specular = glm::vec3(1.0f, 1.0f, 1.0f);
  bunnyMaterial->shininess = 32.0f;
  bunnyMaterial->dissolve = 1.0f;
  bunnyMaterial->diffuseTextureId = -1;

  auto car = asset::loader::Obj::tryFromFile("resources/78717.obj", *renderer->textureManager3D);
  if (car.has_value()) {
    auto assetModel = renderer->createAsset3D(car.value());

    auto modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-5.5f, -30.0f, -4.6f));

    auto ent = registry->create();
    registry->emplace<components::GlobalTransform>(ent, modelMatrix);
    registry->emplace<components::Model3D>(ent, assetModel);
    registry->emplace<components::Material3D>(ent, redMaterial);
  }

  auto out = asset::loader::Obj::tryFromFile("resources/bunny.obj", *renderer->textureManager3D);
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

  renderer->setCameraPos(glm::vec3(-5.0f, 0.0f, 0.0f));

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
