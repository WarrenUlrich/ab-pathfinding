#pragma once

#include "../bank.hpp"

namespace navigation {
  class bank_booth : public bank {
  public:
    bank_booth(const Tile &location, const Tile &stand_tile)
      : bank(location, stand_tile) {}

    bool open() const {
      if (Bank::IsOpen())
        return true;
      
      const auto obj = GameObjects::Get(location);
      if (!obj)
        return false;
      
      if (!obj.Interact("Bank"))
        return false;
      
      return WaitFunc(2000, 50, Bank::IsOpen);
    }
  };
}