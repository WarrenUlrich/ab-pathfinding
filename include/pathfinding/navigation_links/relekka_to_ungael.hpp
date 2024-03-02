#pragma once

#include "../navigation_link.hpp"

namespace pathfinding {
class relekka_to_ungael : public navigation_link {
public:
  relekka_to_ungael() : navigation_link(Tile(2640, 3697, 0), Tile(2277, 4034, 0)) {}
  
  bool handle() const {
    return true;
  }
};
} // namespace pathfinding