#pragma once

#include "../navigation_link.hpp"
#include "../pathfinder_settings.hpp"

namespace navigation {
class gate_nv : public navigation_link {
public:
  // Should be the closed position of the gate
  // as it's tile is used to get the gate object,
  // as well as check the open/closed state.
  Tile tile;

  gate_nv(const Tile &tile, const Tile &from,
                 const Tile &to)
      : tile(tile), navigation_link(from, to) {}

  bool handle() const {
    const auto gate = WallObjects::Get(tile);

    if (!gate) {
      // Gate already open, presumably.
      // Not all gates work this way (alkharid gate)
      return true;
    }

    if (!gate.Interact("Open")) {
      return false;
    }

    return true; 
  }
};
} // namespace pathfinding