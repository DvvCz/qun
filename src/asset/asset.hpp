#pragma once

#include <optional>
#include <string>

namespace asset {
  class Asset {
  public:
    [[nodiscard]] virtual std::optional<std::string> getName() const noexcept = 0;
  };
}
