#pragma once

#include <Game/Core.hpp>

#include <astar/astar.hpp>

#include <array>
#include <tuple>

#include "collision_data.hpp"

namespace ab::pathfinding {
class tile_cost_fn {
public:
  double operator()(const Tile &t1, const Tile &t2) const {
    return t1.DistanceFrom(t2);
  }
};

class tile_heuristic_fn {
public:
  double operator()(const Tile &t1, const Tile &t2) const {
    return t1.DistanceFrom(t2);
  }
};

class tile_neighbors_fn {
public:
  std::vector<Tile> operator()(const Tile &t) const {
    std::vector<Tile> neighbors;

    auto region = ((std::int32_t) (((t.X >> 6) << 8) | (t.Y >> 6)));

    if (mapped_regions.find(region) == mapped_regions.end())
      return {};
      
    constexpr std::array<
        std::tuple<int, int, Pathfinding::COLLISION_FLAG>,
        4>
        directions = {
            {{0, 1, Pathfinding::COLLISION_FLAG::NORTH},
             {1, 0, Pathfinding::COLLISION_FLAG::EAST},
             {0, -1, Pathfinding::COLLISION_FLAG::SOUTH},
             {-1, 0, Pathfinding::COLLISION_FLAG::WEST}}};

    for (const auto [dx, dy, df] : directions) {
      const auto neighbor = Tile{t.X + dx, t.Y + dy, t.Plane};

      auto search = collision_data.find(neighbor);
      if (search == collision_data.end()) {
        neighbors.emplace_back(neighbor);
      } else {
        auto flag = search->second;
        if (!((flag & Pathfinding::BLOCKED) || (flag & Pathfinding::OCCUPIED))) {
          neighbors.emplace_back(neighbor);
        }
      }
    }

    return neighbors;
  }
};

using pathfinder =
    astar::pathfinder<Tile, tile_cost_fn, tile_heuristic_fn,
                      tile_neighbors_fn>;

auto find_path(const Tile &start, const Tile &destination) {
  static pathfinder pf =
      pathfinder{tile_cost_fn{}, tile_heuristic_fn{},
                 tile_neighbors_fn{}};

  return pf.search(start, destination);
}

} // namespace ab::pathfinding