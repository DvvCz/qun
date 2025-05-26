#include "renderer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <print>
#include <entt/entt.hpp>

#include "../shader/shader.hpp"
#include "../shader/program.hpp"
#include "../resource/obj/obj.hpp"

#include "../components/transform.hpp"
#include "../components/model.hpp"
#include "../components/light.hpp"

Renderer::Renderer(const std::shared_ptr<Window>& window,
                   const std::shared_ptr<entt::registry>& registry) /* clang-format off */
  : window(window), registry(registry),
  uniformProjMatrix(0),
  uniformViewMatrix(1),
  uniformModelMatrix(2),
  uniformTextureArray(3),
  uniformTextureIdx(4),
  uniformCameraPos(5),
  uniformLightBlock(0),
  uniformMaterialBlock(1)
{ /* clang-format on */
  cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
  cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);

  // Use the fixed 16:9 aspect ratio instead of calculating from window size
  projMatrix = glm::perspective(glm::radians(45.0f), ASPECT_RATIO, 0.1f, 100.0f);
  viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, upDir);

  modelMatrix = glm::mat4(1.0f);
  modelMatrix = glm::translate(modelMatrix, glm::vec3(5.0f, 0.0f, 0.0f));
  modelMatrix = glm::scale(modelMatrix, glm::vec3(20, 20, 20));
  modelMatrix = glm::rotate(modelMatrix, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

  // Set initial viewport size with 16:9 aspect ratio
  int framebufferWidth, framebufferHeight;
  glfwGetFramebufferSize(window->getGlfwWindow(), &framebufferWidth, &framebufferHeight);

  // Calculate viewport dimensions to maintain 16:9 aspect ratio
  float currentAspectRatio = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);

  int viewportX = 0;
  int viewportY = 0;
  int viewportWidth = framebufferWidth;
  int viewportHeight = framebufferHeight;

  // If current aspect ratio is wider than target, apply pillarboxing
  if (currentAspectRatio > ASPECT_RATIO) {
    viewportWidth = static_cast<int>(framebufferHeight * ASPECT_RATIO);
    viewportX = (framebufferWidth - viewportWidth) / 2;
  }
  // If current aspect ratio is taller than target, apply letterboxing
  else if (currentAspectRatio < ASPECT_RATIO) {
    viewportHeight = static_cast<int>(framebufferWidth / ASPECT_RATIO);
    viewportY = (framebufferHeight - viewportHeight) / 2;
  }

  glViewport(viewportX, viewportY, viewportWidth, viewportHeight);

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glDebugMessageCallback([](/* clang-format off */
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
  ) {
    std::println(stderr, "OpenGL Error: {}", message);
  }, nullptr); /* clang-format on */

  {
    auto fragShader = std::make_unique<shader::Shader>(std::filesystem::path("shaders/main.frag"), shader::Type::Fragment);
    auto vertShader = std::make_unique<shader::Shader>(std::filesystem::path("shaders/main.vert"), shader::Type::Vertex);

    shader3D = std::make_unique<shader::Program>();
    shader3D->addShader(std::move(vertShader));
    shader3D->addShader(std::move(fragShader));
    shader3D->link();
  }

  {
    auto fragShader = std::make_unique<shader::Shader>(std::filesystem::path("shaders/main2d.frag"), shader::Type::Fragment);
    auto vertShader = std::make_unique<shader::Shader>(std::filesystem::path("shaders/main2d.vert"), shader::Type::Vertex);

    shader2D = std::make_unique<shader::Program>();
    shader2D->addShader(std::move(vertShader));
    shader2D->addShader(std::move(fragShader));
    shader2D->link();
  }

  textureManager = std::make_shared<TextureManager>(uniformTextureArray, uniformTextureIdx);

  // todo: probably only store the uniform in the material manager itself
  materialManager = std::make_shared<MaterialManager>(uniformMaterialBlock, textureManager);

  // Need to activate shader program before setting uniforms
  shader3D->use();

  uniformLightBlock.set({.lightCount = 0});
  uniformCameraPos.set(cameraPos);
}

MaterialBlock defaultMaterial = {/* clang-format off */
  .ambient = glm::vec3(0.2f, 0.2f, 0.2f),
  .diffuse = glm::vec3(0.8f, 0.8f, 0.8f),
  .specular = glm::vec3(1.0f, 1.0f, 1.0f),
  .shininess = 32.0f,
  .dissolve = 1.0f
};/* clang-format on */

void Renderer::drawFrame() {
  glClearColor(0.3f, 0.3f, 0.6f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth buffer

  shader2D->use();
#ifdef SHADER_HOTRELOADING
  shader2D->checkForHotReload();
#endif

  auto ents2d = registry->view<components::GlobalTransform, components::Model2D>();
  for (const auto ent : ents2d) {
    auto model = registry->get<components::Model2D>(ent);
    model->draw();
  }

  shader3D->use();
#ifdef SHADER_HOTRELOADING
  shader3D->checkForHotReload();
#endif

  uniformProjMatrix.set(projMatrix);
  uniformViewMatrix.set(viewMatrix);

  auto lightEnts = registry->view<components::GlobalTransform, components::Light>();
  lightBlock.lightCount = 0;

  for (const auto ent : lightEnts) {
    if (lightBlock.lightCount >= MAX_LIGHTS) {
      std::println(stderr, "Maximum number of lights exceeded: {}", MAX_LIGHTS);
      break; // Prevent overflow
    }

    auto light = registry->get<components::Light>(ent);
    auto globalTransform = registry->get<components::GlobalTransform>(ent);

    lightBlock.lights[lightBlock.lightCount++] = {/* clang-format off */
      .position = glm::vec3(globalTransform[3]),
      .color = light.color * light.intensity
    }; /* clang-format on */
  }

  uniformLightBlock.set(lightBlock);

  auto ents3d = registry->view<components::GlobalTransform, components::Model3D>();
  for (const auto ent : ents3d) {
    auto globalTransform = registry->get<components::GlobalTransform>(ent);
    auto model = registry->get<components::Model3D>(ent);

    if (registry->all_of<components::Material>(ent)) {
      auto material = registry->get<components::Material>(ent);
      materialManager->setMaterial(*material);
    } else {
      // Use the default material if no specific material is set
      materialManager->setMaterial(defaultMaterial);
    }

    textureManager->unbindTexture();

    modelMatrix = globalTransform;
    uniformModelMatrix.set(modelMatrix);

    model->draw();
  }
}

void Renderer::setProjectionMatrix(const glm::mat4x4& projMatrix) noexcept {
  this->projMatrix = projMatrix;
  uniformProjMatrix.set(projMatrix);
}

void Renderer::setViewMatrix(const glm::mat4x4& viewMatrix) noexcept {
  this->viewMatrix = viewMatrix;
  uniformViewMatrix.set(viewMatrix);
}

void Renderer::setModelMatrix(const glm::mat4x4& modelMatrix) noexcept {
  this->modelMatrix = modelMatrix;
  uniformModelMatrix.set(modelMatrix);
}

void Renderer::setCameraPos(const glm::vec3& cameraPos) noexcept {
  this->cameraPos = cameraPos;
  viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, upDir);

  uniformViewMatrix.set(viewMatrix);
  uniformCameraPos.set(cameraPos);
}

void Renderer::setCameraDir(const glm::vec3& cameraDir) noexcept {
  cameraFront = cameraDir;

  viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, upDir);

  uniformViewMatrix.set(viewMatrix);
}

const glm::mat4x4& Renderer::getProjectionMatrix() const noexcept {
  return projMatrix;
}

const glm::mat4x4& Renderer::getViewMatrix() const noexcept {
  return viewMatrix;
}

const glm::mat4x4& Renderer::getModelMatrix() const noexcept {
  return modelMatrix;
}

const glm::vec3& Renderer::getCameraPos() const noexcept {
  return cameraPos;
}

std::shared_ptr<model::Asset> Renderer::createAssetModel(const resource::ObjAsset& asset) const {
  return std::make_shared<model::Asset>(asset, textureManager, materialManager);
}
