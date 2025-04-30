#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "shader.hpp"

namespace shader {
  class Program {
  public:
    Program();
    ~Program();

    // Add a shader by transferring ownership
    void addShader(std::unique_ptr<Shader> shader);

    void link();
    void use() const;
    bool setUniformMatrix4fv(const std::string& name, const glm::mat4& matrix);
    [[nodiscard]] uint32_t getProgramIdx() const;

  private:
    uint32_t programIdx;
    std::vector<std::unique_ptr<Shader>> shaders;
  };
}