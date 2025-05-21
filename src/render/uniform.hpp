#pragma once

#include <glm/glm.hpp>
#include <string>

class Uniform {
public:
  [[nodiscard]] virtual int getLocation() const = 0;
  [[nodiscard]] virtual const std::string& getName() const = 0;

  virtual void addToProgram() const = 0;
};

class Mat4x4Uniform : public Uniform {
public:
  Mat4x4Uniform(std::string name, int location, const glm::mat4x4 value);

  [[nodiscard]] int getLocation() const;
  [[nodiscard]] const std::string& getName() const;
  [[nodiscard]] const glm::mat4x4& getValue() const;

  void addToProgram() const;

private:
  glm::mat4x4 value;
  std::string name;
  int location;
};