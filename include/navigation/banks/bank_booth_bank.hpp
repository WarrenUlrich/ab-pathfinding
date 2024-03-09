#pragma once

#include "../bank.hpp"

namespace navigation {
class bank_booth_bank : public bank {
public:
  bank_booth_bank(const std::string &name, const Area &area,
                  const Tile &pathfinding_tile);

  bool open() const override;

  ~bank_booth_bank() = default;
};
} // namespace pathfinding