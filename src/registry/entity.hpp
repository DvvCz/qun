#include <glm/glm.hpp>

#include "../resource/obj/obj.hpp"

class Entity {
public:
  Entity(int id);

  void setPosition(const glm::vec3& position);
  void setRotation(const glm::vec3& rotation);
  void setScale(const glm::vec3& scale);

  [[nodiscard]] int getId() const;
  [[nodiscard]] const glm::vec3& getPosition() const;
  [[nodiscard]] const glm::vec3& getRotation() const;
  [[nodiscard]] const glm::vec3& getScale() const;
  [[nodiscard]] const std::shared_ptr<resource::ObjAsset> getAsset() const;

  [[nodiscard]] glm::mat4x4 getModelMatrix() const;

private:
  int id;
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;
  std::shared_ptr<resource::ObjAsset> asset;
};
