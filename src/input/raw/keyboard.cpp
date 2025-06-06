#include "keyboard.hpp"

#include <print>
#include <algorithm>

namespace input {
  Keyboard::KeyMap Keyboard::keysPressedNow = {false};
  Keyboard::KeyMap Keyboard::keysReleasedNow = {false};
  Keyboard::KeyMap Keyboard::keysBeingHeld = {false};

  void Keyboard::bindGlfwCallbacks(GLFWwindow* window) {
    glfwSetKeyCallback(window, onGlfwKeyCallback);
  }

  void Keyboard::onKeyPressed(const Key key) {
    auto keyIndex = static_cast<size_t>(key);

    keysPressedNow.at(keyIndex) = true;
    keysReleasedNow.at(keyIndex) = false;
    keysBeingHeld.at(keyIndex) = true;
  }

  void Keyboard::onKeyReleased(const Key key) {
    auto keyIndex = static_cast<size_t>(key);

    keysPressedNow.at(keyIndex) = false;
    keysReleasedNow.at(keyIndex) = true;
    keysBeingHeld.at(keyIndex) = false;
  }

  void Keyboard::onGlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      onKeyPressed(static_cast<Key>(key));
    }

    if (action == GLFW_RELEASE) {
      onKeyReleased(static_cast<Key>(key));
    }
  }

  bool Keyboard::wasPressedNow(const Key key) {
    return keysPressedNow.at(static_cast<size_t>(key));
  }

  bool Keyboard::wasReleasedNow(const Key key) {
    return keysReleasedNow.at(static_cast<size_t>(key));
  }

  bool Keyboard::isBeingHeld(const Key key) {
    return keysBeingHeld.at(static_cast<size_t>(key));
  }

  void Keyboard::resetCurrentKeyMaps() {
    std::fill(keysPressedNow.begin(), keysPressedNow.end(), false);
    std::fill(keysReleasedNow.begin(), keysReleasedNow.end(), false);
  }
}
