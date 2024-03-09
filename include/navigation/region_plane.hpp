#pragma once

#include <cstdint>
#include <utility>

namespace navigation {
class region_plane {
public:
  std::int32_t region;
  std::int32_t plane;

  region_plane(std::int32_t region, std::int32_t plane)
      : region(region), plane(plane) {}

  region_plane(const region_plane &other) = default;

  region_plane(region_plane &&other) = default;

  bool operator==(const region_plane &other) const {
    return other.plane == plane && other.region == region;
  }

  bool operator!=(const region_plane &other) const {
    return !(*this == other);
  }
};
} // namespace pathfinding

namespace std {
template <> struct hash<navigation::region_plane> {
  std::size_t
  operator()(const navigation::region_plane &rp) const {
    std::size_t h1 = std::hash<std::int32_t>()(rp.region);
    std::size_t h2 = std::hash<std::int32_t>()(rp.plane);
    return h1 ^ (h2 << 1);
  }
};
} // namespace std