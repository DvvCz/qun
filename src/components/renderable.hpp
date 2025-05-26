#pragma once

#include <memory>

#include "../render/model/model.hpp"

namespace components {
  using Renderable = std::shared_ptr<Model>;
}
