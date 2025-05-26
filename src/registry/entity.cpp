#include "entity.hpp"

Entity::Entity(int id) : id(id), position(0.0f), rotation(0.0f), scale(1.0f) {
}

void Entity::setPosition(const glm::vec3& position) {
  this->position = position;
}

void Entity::setRotation(const glm::vec3& rotation) {
  this->rotation = rotation;
}

void Entity::setScale(const glm::vec3& scale) {
  this->scale = scale;
}

int Entity::getId() const {
  return id;
}

const glm::vec3& Entity::getPosition() const {
  return position;
}

const glm::vec3& Entity::getRotation() const {
  return rotation;
}

const glm::vec3& Entity::getScale() const {
  return scale;
}

const std::shared_ptr<resource::ObjAsset> Entity::getAsset() const {
  return asset;
}
