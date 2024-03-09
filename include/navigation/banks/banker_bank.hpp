#pragma once

#include "../bank.hpp"

namespace navigation {
class banker_bank : public bank {
public:
  banker_bank(const std::string &name, const Area &area,
                  const Tile &pathfinding_tile);

  bool open() const override;

  ~banker_bank() = default;
};
} // namespace pathfinding