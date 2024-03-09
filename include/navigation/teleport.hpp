#pragma once

#include <Core/Types/Tile.hpp>

#include "tile_hash.hpp"

namespace navigation {
class teleport {
public:
  Tile to;

  teleport(const Tile &to) : to(to) {}

  virtual bool available(const Tile &from) const = 0;

  virtual bool use() const = 0;
};
} // namespace pathfinding

namespace std {
template <> struct hash<navigation::teleport> {
  std::size_t
  operator()(const navigation::teleport &tele) const {
    return std::hash<Tile>()(tele.to);
  }
};
} // namespace std