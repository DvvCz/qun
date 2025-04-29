#include "renderer.hpp"

#include "../shader/shader.hpp"
#include "../shader/program.hpp"
#include <glm/glm.hpp>
#include <print>

struct Vertex {
  glm::vec3 vertPos;
  glm::vec3 vertNormal;
};

// Create vertex data with positions and normals
Vertex vertices[] = {/* clang-format off */
  {
    .vertPos = glm::vec3(-0.5f, -0.5f, 0.0f),
    .vertNormal = glm::vec3(0.0f, 0.0f, 1.0f)
  },
  {
    .vertPos = glm::vec3(0.5f, -0.5f, 0.0f),
    .vertNormal = glm::vec3(0.0f, 0.0f, 1.0f)
  },
  {
    .vertPos = glm::vec3(0.0f, 0.5f, 0.0f),
    .vertNormal = glm::vec3(0.0f, 0.0f, 1.0f)
  }
}; /* clang-format on */

Renderer::Renderer(const std::shared_ptr<Window>& window) : window(window) {
  auto fragShader = std::make_unique<shader::Shader>(std::filesystem::path("../src/shader/shaders/basic.frag"),
                                                     shader::ShaderType::Fragment);

  auto vertShader = std::make_unique<shader::Shader>(std::filesystem::path("../src/shader/shaders/basic.vert"),
                                                     shader::ShaderType::Vertex);

  glm::mat4x4 projMatrix;
  glm::mat4x4 viewMatrix;
  glm::mat4x4 modelMatrix;

  glEnable(GL_DEBUG_OUTPUT);
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

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  // VAO
  glBindVertexArray(vao);

  // VBO
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Position attribute (location=0)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vertPos));
  glEnableVertexAttribArray(0);

  // Normal attribute (location=1)
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, vertNormal));
  glEnableVertexAttribArray(1);
}

void Renderer::drawFrame() const {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(shaderProgram->getProgramIdx());
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}