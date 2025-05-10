#include "obj.hpp"

#include <variant>
#include <print>

std::expected<void, std::string> resource::Obj::consumeComment(size_t& ptr, const std::vector<std::byte>& buffer) noexcept {
  while (ptr < buffer.size() && buffer.at(ptr++) != (std::byte)'\n') {
  }

  return {};
}

std::expected<float, std::string> resource::Obj::consumeFloat(size_t& ptr, const std::vector<std::byte>& buffer) noexcept {
  std::string number;
  size_t start = ptr;

  while (ptr < buffer.size()) {
    std::byte currentByte = buffer.at(ptr);

    if (currentByte == (std::byte)' ' || currentByte == (std::byte)'\n') {
      break;
    }

    number += static_cast<char>(currentByte);
    ptr++;
  }

  if (number.empty()) {
    ptr = start;
    return std::unexpected(std::format("Failed to parse float at {}", ptr));
  }

  try {
    return std::stof(number);
  } catch (const std::exception&) {
    ptr = start;
    return std::unexpected(std::format("Failed to convert '{}' to float at {}", number, ptr));
  }
}

std::expected<int, std::string> resource::Obj::consumeInt(size_t& ptr, const std::vector<std::byte>& buffer) noexcept {
  std::string number;
  size_t start = ptr;

  while (ptr < buffer.size()) {
    std::byte currentByte = buffer.at(ptr);

    if (currentByte < (std::byte)'0' && currentByte > (std::byte)'9' && currentByte != (std::byte)'-') {
      break;
    }

    number += static_cast<char>(currentByte);
    ptr++;
  }

  if (number.empty()) {
    ptr = start;
    return std::unexpected(std::format("Failed to parse int at {}", ptr));
  }

  return std::stoi(number);
}

std::expected<resource::Obj::Vertex, std::string> resource::Obj::consumeVertex(size_t& ptr,
                                                                               const std::vector<std::byte>& buffer) noexcept {
  resource::Obj::Vertex vertex;

  consumeWhitespace(ptr, buffer);

  auto x = consumeFloat(ptr, buffer);
  if (!x.has_value()) {
    return std::unexpected(x.error());
  }

  consumeWhitespace(ptr, buffer);

  auto y = consumeFloat(ptr, buffer);
  if (!y.has_value()) {
    return std::unexpected(y.error());
  }

  consumeWhitespace(ptr, buffer);

  auto z = consumeFloat(ptr, buffer);
  if (!z.has_value()) {
    return std::unexpected(z.error());
  }

  consumeWhitespace(ptr, buffer);

  auto w = consumeFloat(ptr, buffer);
  if (w.has_value()) {
    vertex.w = w.value();
  } else {
    vertex.w = 1.0f;
  }

  vertex.x = x.value();
  vertex.y = y.value();
  vertex.z = z.value();

  return vertex;
}

std::expected<resource::Obj::Normal, std::string> resource::Obj::consumeNormal(size_t& ptr,
                                                                               const std::vector<std::byte>& buffer) noexcept {

  consumeWhitespace(ptr, buffer);

  resource::Obj::Normal normal;

  auto x = consumeFloat(ptr, buffer);
  if (!x.has_value()) {
    return std::unexpected(x.error());
  }

  consumeWhitespace(ptr, buffer);

  auto y = consumeFloat(ptr, buffer);
  if (!y.has_value()) {
    return std::unexpected(y.error());
  }

  consumeWhitespace(ptr, buffer);

  auto z = consumeFloat(ptr, buffer);
  if (!z.has_value()) {
    return std::unexpected(z.error());
  }

  normal.x = x.value();
  normal.y = y.value();
  normal.z = z.value();

  return normal;
}

std::expected<resource::Obj::TexCoord, std::string>
resource::Obj::consumeTexCoord(size_t& ptr, const std::vector<std::byte>& buffer) noexcept {
  consumeWhitespace(ptr, buffer);

  resource::Obj::TexCoord texCoord;

  auto u = consumeFloat(ptr, buffer);
  if (!u.has_value()) {
    return std::unexpected(u.error());
  }

  consumeWhitespace(ptr, buffer);

  auto v = consumeFloat(ptr, buffer);
  if (!v.has_value()) {
    return std::unexpected(v.error());
  }

  consumeWhitespace(ptr, buffer);

  auto w = consumeFloat(ptr, buffer);
  if (w.has_value()) {
    texCoord.w = w.value();
  } else {
    texCoord.w = 0.0f;
  }

  texCoord.u = u.value();
  texCoord.v = v.value();

  return texCoord;
}

std::expected<resource::Obj::Face, std::string> resource::Obj::consumeFace(size_t& ptr,
                                                                           const std::vector<std::byte>& buffer) noexcept {
  resource::Obj::Face face;

  for (int i = 0; i < 3; ++i) {
    auto vertexIndex = consumeInt(ptr, buffer);
    if (!vertexIndex.has_value()) {
      return std::unexpected(vertexIndex.error());
    }

    face.vertexIndices[i] = vertexIndex.value();

    if (ptr < buffer.size() && buffer.at(ptr) == (std::byte)'/') {
      auto texCoordIndex = consumeInt(++ptr, buffer);

      if (!texCoordIndex.has_value()) {
        face.texCoordIndices[i] = -1;
      } else {
        face.texCoordIndices[i] = texCoordIndex.value();
      }
    }

    if (ptr < buffer.size() && buffer.at(ptr) == (std::byte)'/') {
      auto normalIndex = consumeInt(++ptr, buffer);

      if (!normalIndex.has_value()) {
        std::println("the fuck");
        return std::unexpected(normalIndex.error());
      }

      face.normalIndices[i] = normalIndex.value();
    }
  }

  return face;
}

std::expected<resource::Obj::Line, std::string> resource::Obj::consumeLine(size_t& ptr,
                                                                           const std::vector<std::byte>& buffer) noexcept {
  resource::Obj::Line line;

  while (ptr < buffer.size()) {
    auto vertexIndex = consumeInt(ptr, buffer);
    if (!vertexIndex.has_value()) {
      return std::unexpected(vertexIndex.error());
    }

    line.vertexIndices.push_back(vertexIndex.value());

    if (ptr < buffer.size() && buffer.at(ptr) == (std::byte)' ') {
      ++ptr;
    } else {
      break;
    }
  }

  return line;
}

void resource::Obj::consumeWhitespace(size_t& ptr, const std::vector<std::byte>& buffer) noexcept {
  while (ptr < buffer.size()) {
    std::byte currentByte = buffer.at(ptr);

    if (currentByte != (std::byte)' ' && currentByte != (std::byte)'\n' && currentByte != (std::byte)'\t') {
      break;
    }

    ptr++;
  }
}

std::expected<std::string, std::string> resource::Obj::consumeIdent(size_t& ptr,
                                                                    const std::vector<std::byte>& buffer) noexcept {
  std::string ident;

  while (ptr < buffer.size()) {
    std::byte currentByte = buffer.at(ptr);

    if (currentByte == (std::byte)' ' || currentByte == (std::byte)'\n') {
      break;
    }

    ident += static_cast<char>(currentByte);
    ptr++;
  }

  return ident;
}

std::expected<Asset, std::string> resource::Obj::tryFromBuffer(const std::vector<std::byte>& buffer) noexcept {
  size_t ptr = 0;

  std::vector<Vertex> vertices;
  std::vector<Normal> normals;
  std::vector<TexCoord> texCoords;

  std::vector<Face> faces;
  std::vector<Line> lines;

  while (true) {
    consumeWhitespace(ptr, buffer);

    if (ptr >= buffer.size()) {
      break;
    }

    std::byte currentByte = buffer.at(ptr);

    std::println("Current byte: {}", static_cast<char>(currentByte));

    switch (currentByte) {
    case (std::byte)'#':
      consumeComment(++ptr, buffer);
      break;
    case (std::byte)'v': {
      if (++ptr >= buffer.size()) {
        return std::unexpected("Invalid vertex data");
      }

      if (buffer.at(ptr) == (std::byte)'t') {
        consumeTexCoord(++ptr, buffer);
        break;
      }

      if (buffer.at(ptr) == (std::byte)'n') {
        consumeNormal(++ptr, buffer);
        break;
      }

      auto vertex = consumeVertex(ptr, buffer);

      if (!vertex.has_value()) {
        return std::unexpected(vertex.error());
      }

      vertices.push_back(vertex.value());

      break;
    }
    case (std::byte)'f': {
      auto face = consumeFace(++ptr, buffer);

      if (!face.has_value()) {
        return std::unexpected(face.error());
      }

      faces.push_back(face.value());
      break;
    }
    case (std::byte)'l': {
      auto line = consumeLine(++ptr, buffer);

      if (!line.has_value()) {
        return std::unexpected(line.error());
      }

      lines.push_back(line.value());
      break;
    }
    case (std::byte)'o': {
      consumeWhitespace(++ptr, buffer);

      auto objName = consumeIdent(ptr, buffer);
      if (!objName.has_value()) {
        return std::unexpected(objName.error());
      }

      std::println("Object name: {}", objName.value());
      break;
    }
    case (std::byte)'s': {
      consumeWhitespace(++ptr, buffer);

      auto shading = consumeIdent(ptr, buffer);
      if (!shading.has_value()) {
        return std::unexpected(shading.error());
      }

      std::println("Shading: {}", shading.value());
      break;
    }
    default: {
      auto command = consumeIdent(ptr, buffer);
      if (!command.has_value()) {
        return std::unexpected(command.error());
      }

      if (command.value() == "mtllib") {
        consumeWhitespace(ptr, buffer);

        auto mtlFileName = consumeIdent(ptr, buffer);
        if (!mtlFileName.has_value()) {
          return std::unexpected(mtlFileName.error());
        }

        std::println("Material library: {}", mtlFileName.value());
        break;
      } else if (command.value() == "usemtl") {
        consumeWhitespace(ptr, buffer);

        auto mtlName = consumeIdent(ptr, buffer);
        if (!mtlName.has_value()) {
          return std::unexpected(mtlName.error());
        }

        std::println("Using material: {}", mtlName.value());
        break;
      }

      return std::unexpected(std::format("Unknown command: {} at {}", command.value(), ptr));
    }
    }
  }

  return std::unexpected("Not implemented");
}