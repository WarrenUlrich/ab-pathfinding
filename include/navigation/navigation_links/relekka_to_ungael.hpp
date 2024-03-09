#pragma once

#include "../navigation_link.hpp"

namespace navigation {
class relekka_to_ungael : public navigation_link {
public:
  relekka_to_ungael() : navigation_link(Tile(2640, 3697, 0), Tile(2277, 4034, 0)) {}
  
  bool handle() const {
    const auto boat = GameObjects::Get("Fremennik Boat");
    if (!boat)
      return false;
    
    if (!boat.Interact("Travel"))
      return false;

    return WaitFunc(5000, 50, [&]() {
      return Minimap::GetPosition().DistanceFrom(to) < 4;
    });
  }
};
} // namespace pathfinding