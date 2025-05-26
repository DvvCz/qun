#include "uniform.hpp"

#include "renderer.hpp"

#include "render/material/material2d.hpp"
#include "render/material/material3d.hpp"

template <typename T> UniformBlock<T>::UniformBlock(const GLint location) : location(location) {
  glCreateBuffers(1, &bufferIdx);
  glNamedBufferStorage(bufferIdx, sizeof(T), nullptr, GL_DYNAMIC_STORAGE_BIT);
  glBindBufferBase(GL_UNIFORM_BUFFER, location, bufferIdx);
}

template <typename T> void UniformBlock<T>::set(const T& value) const {
  glNamedBufferSubData(bufferIdx, 0, sizeof(T), &value);
}

template class UniformBlock<LightBlock>;

template class UniformBlock<material::Block2D>;
template class UniformBlock<material::Block3D>;
