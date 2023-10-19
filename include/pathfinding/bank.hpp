#pragma once

#include <Game/Core.hpp>

namespace ab::pathfinding {
class bank {
public:
  Area area;
  Tile pathing_tile;
};

static bank varrock_west_bank{
    Area(Tile(1111, 1111, 0), Tile(1111, 1111, 0)),
    Tile(1111, 1111, 0)};

} // namespace ab::pathfinding