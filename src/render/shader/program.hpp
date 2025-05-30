#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "shader.hpp"

namespace shader {
  struct ShaderProgramSlot {
#ifdef SHADER_HOTRELOADING
    uint64_t fsLastChanged;
#endif
    std::unique_ptr<Shader> shader;
  };

  class Program {
  public:
    Program();
    ~Program();

    // Add a shader by transferring ownership
    void addShader(std::unique_ptr<Shader> shader);

    void link();
    void use();
    [[nodiscard]] uint32_t getProgramIdx() const;

    void checkForHotReload();

  private:
    uint32_t programIdx;
    std::vector<ShaderProgramSlot> shaders;
#ifdef SHADER_HOTRELOADING
    uint64_t lastCheckedForHotReload = 0;
#endif
  };
}
