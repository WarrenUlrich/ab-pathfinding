#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Core/Types/Area.hpp>
#include <Core/Types/Tile.hpp>

namespace pathfinding {
class bank {
public:
  std::string name;
  Area area;
  Tile pathfinding_tile;

  bank() = default;

  bank(const bank &other);

  bank(const std::string &name, const Area &area,
       const Tile &pathfinding_tile);

  bool at() const;

  virtual bool can_use() const;

  virtual bool open() const = 0;

  virtual ~bank() = default;

  using bank_vec = std::vector<std::shared_ptr<bank>>;

  static const bank_vec banks;

  static bank_vec::const_iterator get_closest();
};
} // namespace pathfinding