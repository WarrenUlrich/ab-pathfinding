#pragma once

#include <functional>

#include <Core/Types/Tile.hpp>

namespace std {
template <> struct hash<Tile> {
  std::size_t operator()(const Tile &tile) const {
    std::size_t h1 = std::hash<std::int32_t>()(tile.X);
    std::size_t h2 = std::hash<std::int32_t>()(tile.Y);
    std::size_t h3 = std::hash<std::int32_t>()(tile.Plane);
    return h1 ^ (h2 << 1) ^ (h3 << 2);
  }
};
} // namespace std