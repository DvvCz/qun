#include "renderer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <print>
#include <entt/entt.hpp>

#include "render/material/material2d.hpp"
#include "shader/shader.hpp"
#include "shader/program.hpp"
#include "asset/obj/obj.hpp"

#include "components/transform.hpp"
#include "components/material.hpp"
#include "components/model.hpp"
#include "components/light.hpp"

Renderer::Renderer(const std::shared_ptr<Window>& window,
                   const std::shared_ptr<entt::registry>& registry) /* clang-format off */
  : window(window), registry(registry),
  // 3d
  uniformProjMatrix3D(0),
  uniformViewMatrix3D(1),
  uniformModelMatrix3D(2),
  uniformTextureArray3D(3),
  uniformCameraPos3D(4),
  // 3d - blocks
  uniformLightBlock3D(0),
  uniformMaterialBlock3D(1),

  // 2d
  uniformTextureArray2D(0),
  // 2d - blocks
  uniformMaterialBlock2D(0)
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

  textureManager2D = std::make_shared<texture::Manager>(uniformTextureArray2D);
  textureManager3D = std::make_shared<texture::Manager>(uniformTextureArray3D);

  // todo: probably only store the uniform in the material manager itself
  materialManager2D = std::make_shared<material::Manager2D>(uniformMaterialBlock2D, textureManager2D);
  materialManager3D = std::make_shared<material::Manager3D>(uniformMaterialBlock3D, textureManager3D);

  // Need to activate shader program before setting uniforms
  shader3D->use();

  uniformCameraPos3D.set(cameraPos);
}

material::Block3D defaultMaterial3D = {/* clang-format off */
  .ambient = glm::vec3(0.2f, 0.2f, 0.2f),
  .diffuse = glm::vec3(0.8f, 0.8f, 0.8f),
  .specular = glm::vec3(1.0f, 1.0f, 1.0f),
  .shininess = 32.0f,
  .dissolve = 1.0f,
  .diffuseTextureId = -1
};/* clang-format on */

material::Block2D defaultMaterial2D = {/* clang-format off */
  .color = glm::vec3(1.0f, 1.0f, 1.0f)
};/* clang-format on */

void Renderer::draw2D() {
  textureManager2D->bind();

  shader2D->use();
#ifdef SHADER_HOTRELOADING
  shader2D->checkForHotReload();
#endif

  auto ents2d = registry->view<components::GlobalTransform, components::Model2D>();
  for (const auto ent : ents2d) {
    auto globalTransform = registry->get<components::GlobalTransform>(ent);
    auto model = registry->get<components::Model2D>(ent);

    if (registry->all_of<components::Material2D>(ent)) {
      auto material = registry->get<components::Material2D>(ent);
      materialManager2D->setMaterial(*material);
    } else {
      // Use the default material if no specific material is set
      materialManager2D->setMaterial(defaultMaterial2D);
    }

    model->draw();
  }

  textureManager2D->unbind();
}

void Renderer::draw3D() {
  textureManager3D->bind();

  shader3D->use();
#ifdef SHADER_HOTRELOADING
  shader3D->checkForHotReload();
#endif

  uniformProjMatrix3D.set(projMatrix);
  uniformViewMatrix3D.set(viewMatrix);

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
      .color = light.color * light.intensity,
      .radius = light.radius
    }; /* clang-format on */
  }

  uniformLightBlock3D.set(lightBlock);

  auto ents3d = registry->view<components::GlobalTransform, components::Model3D>();
  for (const auto ent : ents3d) {
    auto globalTransform = registry->get<components::GlobalTransform>(ent);
    auto model = registry->get<components::Model3D>(ent);

    if (registry->all_of<components::Material3D>(ent)) {
      auto material = registry->get<components::Material3D>(ent);
      materialManager3D->setMaterial(*material);
    } else {
      // Use the default material if no specific material is set
      materialManager3D->setMaterial(defaultMaterial3D);
    }

    // textureManager3D->unbindTexture();

    modelMatrix = globalTransform;
    uniformModelMatrix3D.set(modelMatrix);

    model->draw();
  }

  textureManager3D->unbind();
}

void Renderer::drawFrame() {
  glClearColor(0.3f, 0.3f, 0.6f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth buffer

  draw3D();
  draw2D();
}

void Renderer::setCameraPos(const glm::vec3& cameraPos) noexcept {
  this->cameraPos = cameraPos;
  viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, upDir);

  shader3D->use();
  uniformViewMatrix3D.set(viewMatrix);
  uniformCameraPos3D.set(cameraPos);
}

void Renderer::setCameraDir(const glm::vec3& cameraDir) noexcept {
  cameraFront = cameraDir;

  shader3D->use();
  viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, upDir);
  uniformViewMatrix3D.set(viewMatrix);
}

const glm::vec3& Renderer::getCameraPos() const noexcept {
  return cameraPos;
}

std::shared_ptr<model::Asset> Renderer::createAsset3D(const asset::Asset3D& asset) const {
  return std::make_shared<model::Asset>(asset, textureManager3D, materialManager3D);
}
