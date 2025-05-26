#include "registry.hpp"

Registry::Registry() {
}

std::shared_ptr<Entity> Registry::createEntity() {
  auto entity = std::make_shared<Entity>(entities.size());
  entities.push_back(entity);
  return entity;
}
