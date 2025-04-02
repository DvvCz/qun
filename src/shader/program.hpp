#pragma once

#include <cstdint>
#include <vector>
#include "shader.hpp"

namespace shader {
  class Program {
  public:
    Program();
    ~Program();

    void addShader(const Shader& shader);
    void link();
    [[nodiscard]] uint32_t getProgramIdx() const;

  private:
    uint32_t programIdx;
    std::vector<Shader> shaders;
  };
}