#pragma once

#include <Core/Types/Tile.hpp>

#include "tile_hash.hpp"

namespace pathfinding {
class teleport {
public:
  Tile to;

  teleport(const Tile &to) : to(to) {}

  virtual bool available(const Tile &from) const = 0;

  virtual bool use() const = 0;
};
} // namespace pathfinding

namespace std {
template <> struct hash<pathfinding::teleport> {
  std::size_t
  operator()(const pathfinding::teleport &tele) const {
    return std::hash<Tile>()(tele.to);
  }
};
} // namespace std