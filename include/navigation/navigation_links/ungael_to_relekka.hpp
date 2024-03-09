#pragma once

#include "../navigation_link.hpp"

namespace navigation {
class ungael_to_relekka : public navigation_link {
public:
  ungael_to_relekka() : navigation_link(Tile(2277, 4034, 0), Tile(2640, 3697, 0)) {}
  
  bool handle() const {
    return true;
  }
};
} // namespace pathfinding