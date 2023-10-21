#pragma once

#include "navigation_link.hpp"

namespace pathfinding {
class wall_object_nv : public navigation_link {
public:
  Tile tile;

  wall_object_nv(const Tile &tile, const Tile &from,
                 const Tile &to)
      : tile(tile), navigation_link(from, to) {}

  bool handle() const { 
    return true; 
  }
};
} // namespace pathfinding