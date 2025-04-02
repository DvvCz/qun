#include "renderer.hpp"

#include "../shader/shader.hpp"
#include "../shader/program.hpp"

Renderer::Renderer(const std::shared_ptr<Window>& window) : window(window) {
  float triangles[] = {/* clang-format off */
    -0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f
  }; /* clang-format on */

  // VAO
  uint32_t rendererVAO;
  glGenVertexArrays(1, &rendererVAO);
  glBindVertexArray(rendererVAO);

  // VBO
  uint32_t rendererVBO;
  glGenBuffers(1, &rendererVBO);
  glBindBuffer(GL_ARRAY_BUFFER, rendererVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

  shader::Shader fragShader =
      shader::Shader(std::filesystem::path("../src/shader/shaders/basic.frag"), shader::ShaderType::Fragment);

  shader::Shader vertShader =
      shader::Shader(std::filesystem::path("../src/shader/shaders/basic.vert"), shader::ShaderType::Vertex);

  shaderProgram = shader::Program();
  shaderProgram.addShader(vertShader);
  shaderProgram.addShader(fragShader);
  shaderProgram.link();

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  vao = rendererVAO;
  vbo = rendererVBO;
}

void Renderer::drawFrame() const {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // glViewport(0, 0, window->getWidth(), window->getHeight());

  glUseProgram(shaderProgram.getProgramIdx());
  glBindVertexArray(vao);

  glDrawArrays(GL_TRIANGLES, 0, 3);
}