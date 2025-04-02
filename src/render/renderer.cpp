#include "renderer.hpp"

#include "../shader/shader.hpp"
#include "../shader/program.hpp"

float triangles[] = {/* clang-format off */
  -0.5f, -0.5f, 0.0f,
  0.5f, -0.5f, 0.0f,
  0.0f, 0.5f, 0.0f
}; /* clang-format on */

Renderer::Renderer(const std::shared_ptr<Window>& window) : window(window) {
  shader::Shader fragShader =
      shader::Shader(std::filesystem::path("../src/shader/shaders/basic.frag"), shader::ShaderType::Fragment);

  shader::Shader vertShader =
      shader::Shader(std::filesystem::path("../src/shader/shaders/basic.vert"), shader::ShaderType::Vertex);

  shaderProgram = shader::Program();
  shaderProgram.addShader(vertShader);
  shaderProgram.addShader(fragShader);
  shaderProgram.link();

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  // VAO
  glBindVertexArray(vao);

  // VBO
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangles), triangles, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
}

void Renderer::drawFrame() const {
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(shaderProgram.getProgramIdx());
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
}