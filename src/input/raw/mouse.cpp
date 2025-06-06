#include "mouse.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <algorithm>

namespace input {
  Mouse::MouseButtonMap Mouse::mouseButtonsJustPressed = {false};
  Mouse::MouseButtonMap Mouse::mouseButtonsJustReleased = {false};
  Mouse::MouseButtonMap Mouse::mouseButtonsCurrentlyHeld = {false};

  glm::vec2 Mouse::mousePosition = {0.0f, 0.0f};
  glm::vec2 Mouse::mousePositionLastFrame = {0.0f, 0.0f};

  void Mouse::bindGlfwCallbacks(GLFWwindow* const glfwWindow) {
    glfwSetMouseButtonCallback(glfwWindow, onGlfwMouseButtonCallback);
    glfwSetCursorPosCallback(glfwWindow, onGlfwMouseMoveCallback);
  }

  void Mouse::onMouseButtonPressed(const MouseButton button) {
    auto buttonIndex = static_cast<size_t>(button);

    mouseButtonsJustPressed.at(buttonIndex) = true;
    mouseButtonsJustReleased.at(buttonIndex) = false;
    mouseButtonsCurrentlyHeld.at(buttonIndex) = true;
  }

  void Mouse::onMouseButtonReleased(const MouseButton button) {
    auto buttonIndex = static_cast<size_t>(button);

    mouseButtonsJustPressed.at(buttonIndex) = false;
    mouseButtonsJustReleased.at(buttonIndex) = true;
    mouseButtonsCurrentlyHeld.at(buttonIndex) = false;
  }

  glm::vec2 Mouse::getPosition() {
    return mousePosition;
  }

  glm::vec2 Mouse::getPositionDelta() {
    return mousePosition - mousePositionLastFrame;
  }

  void Mouse::onGlfwMouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
    mousePositionLastFrame = mousePosition;

    mousePosition.x = static_cast<float>(xpos);
    mousePosition.y = static_cast<float>(ypos);
  }

  void Mouse::onGlfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
      onMouseButtonPressed(static_cast<MouseButton>(button));
    }

    if (action == GLFW_RELEASE) {
      onMouseButtonReleased(static_cast<MouseButton>(button));
    }
  }

  bool Mouse::wasJustPressed(const MouseButton button) {
    return mouseButtonsJustPressed.at(static_cast<size_t>(button));
  }

  bool Mouse::wasJustReleased(const MouseButton button) {
    return mouseButtonsJustReleased.at(static_cast<size_t>(button));
  }

  bool Mouse::isCurrentlyHeld(const MouseButton button) {
    return mouseButtonsCurrentlyHeld.at(static_cast<size_t>(button));
  }

  void Mouse::resetCurrentMouseMaps() {
    std::fill(mouseButtonsJustPressed.begin(), mouseButtonsJustPressed.end(), false);
    std::fill(mouseButtonsJustReleased.begin(), mouseButtonsJustReleased.end(), false);
    mousePositionLastFrame = mousePosition; // Update last position to current, so delta is zero when no movement
  }
}
