#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cstdint>
#include <string>

class Window final {
public:
  Window(const uint16_t width, const uint16_t height, const std::string title);
  ~Window();

  [[nodiscard]] GLFWwindow* getGlfwWindow() const;
  [[nodiscard]] uint16_t getWidth() const;
  [[nodiscard]] uint16_t getHeight() const;
  [[nodiscard]] std::string& getTitle() const;

  [[nodiscard]] bool shouldClose() const;

private:
  uint16_t currentWidth;
  int currentHeight;
  std::string currentTitle;

  GLFWwindow* glfwWindow;

  static void onResize(GLFWwindow* window, int width, int height);
};
