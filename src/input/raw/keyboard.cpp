#include "keyboard.hpp"

#include <print>
#include <ranges>

namespace Input {
  Keyboard::KeyMap Keyboard::keysJustPressed = {false};
  Keyboard::KeyMap Keyboard::keysJustReleased = {false};
  Keyboard::KeyMap Keyboard::keysCurrentlyHeld = {false};

  void Keyboard::bindGlfwCallbacks(GLFWwindow* window) {
    glfwSetKeyCallback(window, onGlfwKeyCallback);
  }

  void Keyboard::onKeyPressed(const Key key) {
    auto keyIndex = static_cast<size_t>(key);

    keysJustPressed.at(keyIndex) = true;
    keysJustReleased.at(keyIndex) = false;
    keysCurrentlyHeld.at(keyIndex) = true;
  }

  void Keyboard::onKeyReleased(const Key key) {
    auto keyIndex = static_cast<size_t>(key);

    keysJustPressed.at(keyIndex) = false;
    keysJustReleased.at(keyIndex) = true;
    keysCurrentlyHeld.at(keyIndex) = false;
  }

  void Keyboard::onGlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      onKeyPressed(static_cast<Key>(key));
    }

    if (action == GLFW_RELEASE) {
      onKeyReleased(static_cast<Key>(key));
    }
  }

  bool Keyboard::wasJustPressed(const Key key) {
    return keysJustPressed.at(static_cast<size_t>(key));
  }

  bool Keyboard::wasJustReleased(const Key key) {
    return keysJustReleased.at(static_cast<size_t>(key));
  }

  bool Keyboard::isCurrentlyHeld(const Key key) {
    return keysCurrentlyHeld.at(static_cast<size_t>(key));
  }

  void Keyboard::resetCurrentKeyMaps() {
    std::ranges::fill(keysJustPressed, false);
    std::ranges::fill(keysJustReleased, false);
  }
}
