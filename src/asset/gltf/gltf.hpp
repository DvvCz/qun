#pragma once

#include <expected>
#include <string>
#include <filesystem>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>
#include <glm/glm.hpp>

#include "asset/asset.hpp"

#include "render/texture.hpp"

namespace asset::loader {
  class Gltf {
  public:
    /* clang-format off */
    [[nodiscard]] static std::expected<asset::Asset3D, std::string> tryFromFile(
      const std::filesystem::path& path,
      texture::Manager& texMan
    ) noexcept; /* clang-format on */
  private:
    /* clang-format off */
    static std::expected<std::vector<asset::Material>, std::string> tryConvertMaterials(
      const fastgltf::Asset& asset,
      texture::Manager& texMan
    ) noexcept; /* clang-format on */

    /* clang-format off */
    static std::expected<size_t, std::string> tryCreateTexture(
      const fastgltf::Asset& asset,
      const fastgltf::Image& image,
      texture::Manager& texMan
    ) noexcept; /* clang-format on */

    /* clang-format off */
    static std::expected<asset::Shape, std::string> tryConvertNode(
      const fastgltf::Asset& asset,
      const fastgltf::Node& node,
      const fastgltf::Mesh& mesh,
      const glm::mat4x4 worldTransform,
      std::vector<Vertex3D>& allVertices
    ) noexcept; /* clang-format on */

    static glm::vec3 convertFromGLTF(float x, float y, float z) noexcept;

    static glm::vec3 parserVecAsGlm(const fastgltf::math::nvec3& vec) noexcept;
    static glm::vec3 parserVecAsGlm(const fastgltf::math::nvec4& vec) noexcept;
    static glm::mat4x4 parserMatAsGlm(const fastgltf::math::fmat4x4& mat) noexcept;
  };
}
