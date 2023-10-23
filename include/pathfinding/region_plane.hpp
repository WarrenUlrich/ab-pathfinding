#pragma once

namespace pathfinding {
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