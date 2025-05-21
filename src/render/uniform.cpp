#include "uniform.hpp"

#include <glad/glad.h>

Mat4x4Uniform::Mat4x4Uniform(std::string name, int location, const glm::mat4x4 value) {
  this->name = std::move(name);
  this->location = location;
  this->value = value;
}

int Mat4x4Uniform::getLocation() const {
  return location;
}

const std::string& Mat4x4Uniform::getName() const {
  return name;
}

const glm::mat4x4& Mat4x4Uniform::getValue() const {
  return value;
}

void Mat4x4Uniform::addToProgram() const {
  glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}