#pragma once

#include <Game/Core.hpp>

#include <astar/astar.hpp>

#include "pathfinder_settings.hpp"

namespace pathfinding {
using path_step =
    std::variant<Tile, const navigation_link *>;

using path = std::vector<path_step>;

class heuristic_fn {
public:
  heuristic_fn(const pathfinder_settings &pfs)
      : _pfs(pfs) {}

  double operator()(const path_step &a,
                    const path_step &b) const {
    auto get_tile_coords = [](const path_step &step)
        -> std::tuple<int, int, int> {
      if (std::holds_alternative<Tile>(step)) {
        const Tile &tile = std::get<Tile>(step);
        return {tile.X, tile.Y, tile.Plane};
      } else {
        // For navigation_link, you might choose to consider
        // its destination or source
        const navigation_link *link =
            std::get<const navigation_link *>(step);
        return {link->to.X, link->to.Y, link->to.Plane};
      }
    };

    int x1, y1, z1, x2, y2, z2;
    std::tie(x1, y1, z1) = get_tile_coords(a);
    std::tie(x2, y2, z2) = get_tile_coords(b);
    return std::abs(x2 - x1) + std::abs(y2 - y1);
  }

private:
  const pathfinder_settings &_pfs;
};

class neighbors_fn {
public:
  neighbors_fn(const pathfinder_settings &pfs)
      : _pfs(pfs) {}

  std::vector<path_step> operator()(const Tile &t) const {
    std::vector<path_step> neighbors;

    const auto region =
        ((std::int32_t)(((t.X >> 6) << 8) | (t.Y >> 6)));

    const auto rp = region_plane(region, t.Plane);

    // if (mapped_regions.find(rp) == mapped_regions.end())
    //   return {};

    constexpr std::array<
        std::tuple<int, int, Pathfinding::COLLISION_FLAG>,
        4>
        directions = {
            {{0, 1, Pathfinding::COLLISION_FLAG::NORTH},
             {1, 0, Pathfinding::COLLISION_FLAG::EAST},
             {0, -1, Pathfinding::COLLISION_FLAG::SOUTH},
             {-1, 0, Pathfinding::COLLISION_FLAG::WEST}}};

    auto flag = Pathfinding::COLLISION_FLAG::OPEN;
    if (auto search = _pfs.collision_map.find(t);
        search != _pfs.collision_map.end()) {
      flag = search->second;
    }

    for (const auto [dx, dy, df] : directions) {
      if (flag & df)
        continue;

      const auto neighbor =
          Tile{t.X + dx, t.Y + dy, t.Plane};

      auto range =
          _pfs.navigation_links.equal_range(neighbor);
      for (auto it = range.first; it != range.second;
           ++it) {
        neighbors.emplace_back(it->second.get());
      }

      auto search = _pfs.collision_map.find(neighbor);
      if (search == _pfs.collision_map.end()) {
        neighbors.emplace_back(neighbor);
      } else {
        auto flag = search->second;
        if (!((flag & Pathfinding::BLOCKED) ||
              (flag & Pathfinding::OCCUPIED))) {
          neighbors.emplace_back(neighbor);
        }
      }
    }

    return neighbors;
  }

  std::vector<path_step> operator()(
      const std::shared_ptr<navigation_link> &t) const {
    return this->operator()(t->to);
  }

  std::vector<path_step>
  operator()(const path_step &t) const {
    if (auto tile = std::get_if<Tile>(&t)) {
      return this->operator()(*tile);
    } else if (auto nv =
                   std::get_if<const navigation_link *>(
                       &t)) {
      return this->operator()(*nv);
    }

    return {};
  }

private:
  const pathfinder_settings &_pfs;
};

using pathfinder =
    astar::pathfinder<path_step, heuristic_fn, heuristic_fn,
                      neighbors_fn>;

std::optional<path>
find_path(const Tile &from, const Tile &to,
          const pathfinder_settings &settings) {
  auto pf = pathfinder(heuristic_fn(settings),
                       heuristic_fn(settings),
                       neighbors_fn(settings));

  return pf.search(from, to);
}
} // namespace pathfinding