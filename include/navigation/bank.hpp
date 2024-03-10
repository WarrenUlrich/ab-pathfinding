#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Game/Core.hpp>

namespace navigation {
class bank {
public:
  Tile location;
  Tile stand_tile;

  bank(const Tile &location, const Tile &stand_tile)
      : location(location), stand_tile(stand_tile) {}

  virtual bool available() const { return true; }

  virtual bool open() const = 0;
};
} // namespace navigation