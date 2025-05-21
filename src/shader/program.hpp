#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "../render/uniform.hpp"

namespace shader {
  class Program {
  public:
    Program();
    ~Program();

    // Add a shader by transferring ownership
    void addShader(std::unique_ptr<Shader> shader);

    void link();
    void use() const;
    [[nodiscard]] uint32_t getProgramIdx() const;

  private:
    uint32_t programIdx;
    std::vector<std::unique_ptr<Shader>> shaders;
  };
}