#pragma once

#include <Core/Types/Tile.hpp>

#include "tile_hash.hpp"

namespace pathfinding {
class navigation_link {
public:
  Tile from;
  Tile to;

  navigation_link() = default;
  navigation_link(const navigation_link &other) = default;
  navigation_link(navigation_link &&other) = default;
  navigation_link(const Tile &from, const Tile &to)
      : from(from), to(to) {}

  navigation_link &operator=(const navigation_link &other) {
    from = other.from;
    to = other.to;
    return *this;
  }

  navigation_link &operator=(navigation_link &&other) {
    from = std::move(other.from);
    to = std::move(other.to);
    return *this;
  }

  bool operator==(const navigation_link &other) const {
    return from == other.from && to == other.to;
  }

  bool operator!=(const navigation_link &other) const {
    return !(*this == other);
  }

  bool can_handle() const { return true; }
  virtual bool handle() const = 0;

  virtual ~navigation_link() = default;
};
} // namespace pathfinding

namespace std {
template <> struct hash<pathfinding::navigation_link> {
  std::size_t
  operator()(const pathfinding::navigation_link &nv) const {
    std::size_t h1 = std::hash<Tile>()(nv.from);
    std::size_t h2 = std::hash<Tile>()(nv.to);
    return h1 ^ (h2 << 1);
  }
};
} // namespace std