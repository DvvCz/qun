#include "renderer.hpp"

#include "../shader/shader.hpp"
#include "../shader/program.hpp"
#include "../resource/obj/obj.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <print>

Renderer::Renderer(const std::shared_ptr<Window>& window)
    : window(window), triangleModel(TriangleModel(/* clang-format off */
      Vertex{glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f)},
      Vertex{glm::vec3(0.5f, 0.0f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},
      Vertex{glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f)}
  )) {
  auto fragShader =
      std::make_unique<shader::Shader>(std::filesystem::path("../src/shader/shaders/basic.frag"), shader::ShaderType::Fragment);

  auto vertShader =
      std::make_unique<shader::Shader>(std::filesystem::path("../src/shader/shaders/basic.vert"), shader::ShaderType::Vertex);

  glm::vec3 upDir = glm::vec3(0.0f, 0.0f, 1.0f);
  glm::vec3 cameraPos = glm::vec3(0.0f, 3.0f, 0.0f);
  glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

  float aspectRatio = (float)window->getWidth() / (float)window->getHeight();

  glm::mat4x4 projMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
  glm::mat4x4 viewMatrix = glm::lookAt(cameraPos, cameraTarget, upDir);
  glm::mat4x4 modelMatrix = glm::mat4(1.0f);

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

  shaderProgram->use();

  Uniform<glm::mat4x4> uniformProjMatrix(0);
  Uniform<glm::mat4x4> uniformViewMatrix(1);
  Uniform<glm::mat4x4> uniformModelMatrix(2);

  uniformProjMatrix.set(projMatrix);
  uniformViewMatrix.set(viewMatrix);
  uniformModelMatrix.set(modelMatrix);
}

void Renderer::drawFrame() const {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth buffer

  shaderProgram->use();

  // Draw all loaded obj models
  if (!assetModels.empty()) {
    for (const auto& assetModel : assetModels) {
      assetModel->draw();
    }
  }
}

void Renderer::addModel(const resource::ObjAsset& asset) {
  assetModels.push_back(std::make_unique<AssetModel>(asset));
}