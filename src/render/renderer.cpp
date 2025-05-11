#include "renderer.hpp"

#include "../shader/shader.hpp"
#include "../shader/program.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <print>

struct Vertex {
  glm::vec3 pos;
  glm::vec3 normal;
};

// Create vertex data with positions and normals
Vertex vertices[] = {/* clang-format off */
  {
    .pos = glm::vec3(-0.5f, 0.0f, -0.5f),
    .normal = glm::vec3(0.0f, 0.0f, 1.0f)
  },
  {
    .pos = glm::vec3(0.5f, 0.0f, -0.5f),
    .normal = glm::vec3(0.0f, 0.0f, 1.0f)
  },
  {
    .pos = glm::vec3(0.0f, 0.0f, 0.5f),
    .normal = glm::vec3(0.0f, 0.0f, 1.0f)
  }
}; /* clang-format on */

Renderer::Renderer(const std::shared_ptr<Window>& window) : window(window) {
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

  // Set the uniform matrices for the shader program
  shaderProgram->use();
  shaderProgram->setUniformMatrix4fv("projMatrix", projMatrix);
  shaderProgram->setUniformMatrix4fv("viewMatrix", viewMatrix);
  shaderProgram->setUniformMatrix4fv("modelMatrix", modelMatrix);

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  // VAO
  glBindVertexArray(vao);

  // VBO
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Position attribute (location=0)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
  glEnableVertexAttribArray(0);

  // Normal attribute (location=1)
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
  glEnableVertexAttribArray(1);
}

void Renderer::drawObject(const ObjectAsset& object) const {
  // Bind the VAO and draw the object
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(object.getVertices().size()));
}

void Renderer::drawFrame() const {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth buffer

  shaderProgram->use();
  // glBindVertexArray(vao);
  // glDrawArrays(GL_TRIANGLES, 0, 3);

  for (const auto& object : objects) {
    drawObject(object);
  }
}