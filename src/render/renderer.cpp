#include "renderer.hpp"

#include "../shader/shader.hpp"
#include "../shader/program.hpp"
#include "../resource/obj/obj.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <print>

Renderer::Renderer(const std::shared_ptr<Window>& window)
    : window(window), uniformProjMatrix(0), uniformViewMatrix(1), uniformModelMatrix(2), uniformTextureArray(3),
      uniformTextureIdx(4) {
  auto fragShader =
      std::make_unique<shader::Shader>(std::filesystem::path("../src/shader/shaders/basic.frag"), shader::ShaderType::Fragment);

  auto vertShader =
      std::make_unique<shader::Shader>(std::filesystem::path("../src/shader/shaders/basic.vert"), shader::ShaderType::Vertex);

  glm::vec3 upDir = glm::vec3(0.0f, 0.0f, 1.0f);
  glm::vec3 cameraPos = glm::vec3(00.0f, 5.0f, 10.0f);
  glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

  float aspectRatio = (float)window->getWidth() / (float)window->getHeight();

  projMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
  viewMatrix = glm::lookAt(cameraPos, cameraTarget, upDir);
  modelMatrix = glm::mat4(1.0f);

  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEPTH_TEST);

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

  textureManager = std::make_unique<TextureManager>(uniformTextureArray, uniformTextureIdx);
}

void Renderer::drawFrame() const {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth buffer

  shaderProgram->use();

  uniformProjMatrix.set(projMatrix);
  uniformViewMatrix.set(viewMatrix);
  uniformModelMatrix.set(modelMatrix);

  textureManager->bindTexture(0);

  for (const auto& assetModel : assetModels) {
    assetModel->draw();
  }
}

void Renderer::addTexture(const resource::ImgAsset& texture) noexcept {
  auto textureId = textureManager->addTexture(texture);
  if (!textureId.has_value()) {
    std::println("Failed to add texture: {}", textureId.error());
    return;
  }
}

void Renderer::addModel(const resource::ObjAsset& asset) noexcept {
  assetModels.push_back(std::make_unique<AssetModel>(asset));
}