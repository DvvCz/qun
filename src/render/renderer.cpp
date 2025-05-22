#include "renderer.hpp"

#include "../shader/shader.hpp"
#include "../shader/program.hpp"
#include "../resource/obj/obj.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <print>

Renderer::Renderer(const std::shared_ptr<Window>& window) /* clang-format off */
  : window(window),
  uniformProjMatrix(0),
  uniformViewMatrix(1),
  uniformModelMatrix(2),
  uniformTextureArray(3),
  uniformTextureIdx(4),
  uniformCameraPos(5),
  uniformLightBlock(0),
  uniformMaterialBlock(1)
{ /* clang-format on */
  auto fragShader =
      std::make_unique<shader::Shader>(std::filesystem::path("../src/shader/shaders/basic.frag"), shader::ShaderType::Fragment);

  auto vertShader =
      std::make_unique<shader::Shader>(std::filesystem::path("../src/shader/shaders/basic.vert"), shader::ShaderType::Vertex);

  glm::vec3 upDir = glm::vec3(0.0f, 0.0f, 1.0f);
  glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
  cameraPos = glm::vec3(0.0f, 5.0f, 10.0f);

  float aspectRatio = (float)window->getWidth() / (float)window->getHeight();

  projMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
  viewMatrix = glm::lookAt(cameraPos, cameraTarget, upDir);
  modelMatrix = glm::mat4(1.0f);

  modelMatrix = glm::scale(modelMatrix, glm::vec3(20, 20, 20));
  // modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1, 0.1, 0.1));
  modelMatrix = glm::rotate(modelMatrix, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

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

  shaderProgram = std::make_unique<shader::Program>();
  shaderProgram->addShader(std::move(vertShader));
  shaderProgram->addShader(std::move(fragShader));
  shaderProgram->link();

  textureManager = std::make_shared<TextureManager>(uniformTextureArray, uniformTextureIdx);

  // todo: probably only store the uniform in the material manager itself
  materialManager = std::make_shared<MaterialManager>(uniformMaterialBlock, textureManager);

  lightBlock = {/* clang-format off */
      .lightCount = 2,
      .lights = {
          { // Red light at (0, 0, 5)
            glm::vec3(0.0f, 0.0f, 5.0f),
            glm::vec3(1.0f, 0.0f, 0.0f)
          },
          { // Blue light at (0, 5, 5)
            glm::vec3(0.0f, 5.0f, 5.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
          },
      }
  };/* clang-format on */

  uniformLightBlock.set(lightBlock);

  uniformCameraPos.set(cameraPos);
}

void Renderer::drawFrame() const {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth buffer

  shaderProgram->use();

  uniformProjMatrix.set(projMatrix);
  uniformViewMatrix.set(viewMatrix);
  uniformModelMatrix.set(modelMatrix);

  // Set a default material for models without materials
  MaterialBlock defaultMaterial = {/* clang-format off */
    .ambient = glm::vec3(0.2f, 0.2f, 0.2f),
    .diffuse = glm::vec3(0.8f, 0.8f, 0.8f),
    .specular = glm::vec3(1.0f, 1.0f, 1.0f),
    .shininess = 32.0f,
    .dissolve = 0.2f
  };/* clang-format on */
  materialManager->setMaterial(defaultMaterial);

  // Draw all models - each will set its own materials as needed
  for (const auto& assetModel : assetModels) {
    assetModel->draw();
  }
}

void Renderer::addTexture(const resource::ImgAsset& texture) noexcept {
  if (auto result = textureManager->addTexture(texture); result.has_value()) {
    std::println("Added texture with ID {}", result.value());
  } else {
    std::println(stderr, "Failed to add texture: {}", result.error());
  }
}

void Renderer::addModel(const resource::ObjAsset& asset) noexcept {
  auto model = std::make_unique<AssetModel>(asset, this->textureManager, this->materialManager);
  assetModels.push_back(std::move(model));
}
