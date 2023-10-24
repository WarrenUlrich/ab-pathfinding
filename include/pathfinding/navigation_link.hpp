#pragma once

#include <Core/Types/Tile.hpp>

namespace pathfinding {
class navigation_link {
public:
  Tile from;
  Tile to;

  navigation_link() = default;
  navigation_link(const navigation_link &other) = default;
  navigation_link(navigation_link &&other) = default;
  navigation_link(const Tile &from, const Tile &to);
  
  navigation_link &operator=(const navigation_link &other);
  navigation_link &operator=(navigation_link &&other);
  
  bool operator==(const navigation_link &other) const;
  bool operator!=(const navigation_link &other) const;

  virtual bool can_handle() const;
  virtual bool handle() const = 0;
  
  virtual ~navigation_link() = default;
};
} // namespace pathfinding
