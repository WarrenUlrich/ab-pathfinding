#pragma once

#include <Core/Types/Tile.hpp>

namespace pathfinding {
  static std::int32_t tile_to_region(const Tile &tile) {
    return ((std::int32_t) (((tile.X >> 6) << 8) | (tile.Y >> 6)));
  }
}