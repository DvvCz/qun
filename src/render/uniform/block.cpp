#include <glad/glad.h>
#include <print>

#include "render/renderer.hpp"

#include "render/material/material2d.hpp"
#include "render/material/material3d.hpp"

template <typename T> uniform::Block<T>::Block(const GLint location) : location(location) {
  glCreateBuffers(1, &bufferIdx);
  glNamedBufferStorage(bufferIdx, sizeof(T), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

template <typename T> void uniform::Block<T>::set(const T& value) const {
  std::println("Setting uniform block at location {}", location);
  std::println("size: {}", sizeof(T));

  glBindBufferBase(GL_UNIFORM_BUFFER, location, bufferIdx);
  glNamedBufferSubData(bufferIdx, 0, sizeof(T), &value);
}

template class uniform::Block<render::LightsArray>;

template class uniform::Block<material::Material2D>;
template class uniform::Block<material::Material3D>;
