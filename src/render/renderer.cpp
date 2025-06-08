#include "renderer.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>
#include <print>
#include <entt/entt.hpp>

#include "asset/asset.hpp"
#include "render/material/material2d.hpp"
#include "render/shader/shader.hpp"
#include "render/shader/program.hpp"

#include "components/transform.hpp"
#include "components/material.hpp"
#include "components/model.hpp"
#include "components/light.hpp"

#include "constants.hpp"
#include "render/texture.hpp"

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
  uniformLightsArray3D(0),
  uniformMaterial3D(1),

  // 2d
  uniformTextureArray2D(0),
  // 2d - blocks
  uniformMaterial2D(0)
{ /* clang-format on */
  cameraPos = constants::WORLD_ORIGIN;
  cameraFront = constants::WORLD_FORWARD;

  projMatrix = glm::perspective(glm::radians(45.0f), ASPECT_RATIO, 0.05f, 1000.0f);
  viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, constants::WORLD_UP);

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

#ifdef DEBUG
  glEnable(GL_DEBUG_OUTPUT);
#endif

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

  textureManager2D = std::make_shared<texture::Manager>(uniformTextureArray2D, 0);
  textureManager3D = std::make_shared<texture::Manager>(uniformTextureArray3D, 1);

  // todo: probably only store the uniform in the material manager itself
  materialManager2D = std::make_shared<material::Manager2D>(uniformMaterial2D, textureManager2D);
  materialManager3D = std::make_shared<material::Manager3D>(uniformMaterial3D, textureManager3D);

  // Need to activate shader program before setting uniforms
  shader3D->use();

  uniformCameraPos3D.set(cameraPos);
}

asset::Material defaultMaterial3D = {
    /* clang-format off */
  .ambient = glm::vec3(0.2f, 0.2f, 0.2f),
  .diffuse = glm::vec3(0.8f, 0.8f, 0.8f),
  .specular = glm::vec3(1.0f, 1.0f, 1.0f),
  .shininess = 32.0f,
  .dissolve = 1.0f,
};/* clang-format on */

material::Material2D defaultMaterial2D = {/* clang-format off */
  .color = glm::vec3(1.0f, 1.0f, 1.0f)
};/* clang-format on */

void Renderer::draw2D() {
  shader2D->use();

  textureManager2D->bind();

  auto ents2d = registry->view<components::Model2D>();
  for (const auto ent : ents2d) {
    // auto globalTransform = registry->get<components::GlobalTransform>(ent);
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
  shader3D->use();

  textureManager3D->bind();

  uniformProjMatrix3D.set(projMatrix);
  uniformViewMatrix3D.set(viewMatrix);
  uniformCameraPos3D.set(cameraPos);

  auto lightEnts = registry->view<components::Position, components::Light>();
  lightsArray.lightCount = 0;

  for (const auto ent : lightEnts) {
    if (lightsArray.lightCount >= MAX_LIGHTS) {
      break; // Prevent overflow
    }

    auto light = registry->get<components::Light>(ent);
    auto position = registry->get<components::Position>(ent);

    lightsArray.lights[lightsArray.lightCount++] = {/* clang-format off */
      .position = position.value,
      .color = light.color * light.intensity,
      .radius = light.radius
    }; /* clang-format on */
  }

  uniformLightsArray3D.set(lightsArray);

  auto ents3d = registry->view<components::GlobalTransform, components::Model3D>();
  for (const auto ent : ents3d) {
    auto globalTransform = registry->get<components::GlobalTransform>(ent);
    auto model = registry->get<components::Model3D>(ent);

    asset::Material material = defaultMaterial3D;
    if (registry->all_of<components::Material3D>(ent)) {
      material = *registry->get<components::Material3D>(ent);
    }

    materialManager3D->setMaterial(material);

    if (material.isDoubleSided) {
      glDisable(GL_CULL_FACE);
    } else {
      glEnable(GL_CULL_FACE);

      // This doesn't currently work because:
      // - We transform vertices by the transforms provided by the gltf.
      // - Need to change the code so that transforms are stored and applied at runtime
      // - Potentially need to use glFrontFace?
      if (glm::determinant(globalTransform.value) < 0.0f) {
        glCullFace(GL_BACK); // clockwise
      } else {
        glCullFace(GL_FRONT); // counter-clockwise (normal)
      }
    }

    modelMatrix = globalTransform.value;
    uniformModelMatrix3D.set(modelMatrix);

    model->draw();
  }

  textureManager3D->unbind();
}

void Renderer::drawFrame() {
  // glClearColor(0.3f, 0.3f, 0.6f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth buffer

  // todo: make it more clear this is a skybox stage
  glDisable(GL_DEPTH_TEST);
  draw2D();
  glEnable(GL_DEPTH_TEST);

  draw3D();
}

void Renderer::setCameraPos(const glm::vec3& cameraPos) noexcept {
  this->cameraPos = cameraPos;
  viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, constants::WORLD_UP);
}

void Renderer::setCameraDir(const glm::vec3& cameraDir) noexcept {
  this->cameraFront = cameraDir;
  viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, constants::WORLD_UP);
}

const glm::vec3& Renderer::getCameraPos() const noexcept {
  return cameraPos;
}

std::shared_ptr<model::Asset> Renderer::createAsset3D(const asset::Asset3D& asset) const {
  return std::make_shared<model::Asset>(asset, textureManager3D, materialManager3D);
}
