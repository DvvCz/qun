#include "gltf.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

glm::vec3 asset::loader::Gltf::convertFromGLTF(float x, float y, float z) noexcept {
  return glm::vec3(-z, x, y);
};

glm::vec3 asset::loader::Gltf::glmVecAsParserVec(const fastgltf::math::nvec3& vec) noexcept {
  return glm::vec3(vec[0], vec[1], vec[2]);
}

glm::vec3 asset::loader::Gltf::glmVecAsParserVec(const fastgltf::math::nvec4& vec) noexcept {
  return glm::vec3(vec[0], vec[1], vec[2]);
}

glm::mat4x4 asset::loader::Gltf::glmMatAsParserMat(const fastgltf::math::fmat4x4& mat) noexcept {
  return glm::mat4(/* clang-format off */
    mat[0][0], mat[0][1], mat[0][2], mat[0][3],
    mat[1][0], mat[1][1], mat[1][2], mat[1][3],
    mat[2][0], mat[2][1], mat[2][2], mat[2][3],
    mat[3][0], mat[3][1], mat[3][2], mat[3][3]
  );/* clang-format on */
}

glm::quat asset::loader::Gltf::glmQuatAsParserQuat(const fastgltf::math::fquat& quat) noexcept {
  // fastgltf uses w, x, y, z order for quaternions
  return glm::quat(quat[3], quat[0], quat[1], quat[2]);
}
