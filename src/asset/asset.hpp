#pragma once

#include <vector>
#include <optional>
#include <string>
#include <array>

namespace asset {
  class Asset {
  public:
    [[nodiscard]] virtual std::optional<std::string> getName() const noexcept = 0;
  };
}
