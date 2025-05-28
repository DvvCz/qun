#include "error.hpp"

std::string util::error::indent(const std::string& str) {
  std::string result;

  for (size_t i = 0; i < str.length(); ++i) {
    result += str[i];

    if (str[i] == '\n' && i < str.length() - 1) {
      result += '\t';
    }
  }

  return result;
}
