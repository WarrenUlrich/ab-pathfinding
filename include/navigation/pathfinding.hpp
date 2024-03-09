#pragma once

#include <Game/Core.hpp>

#include <astar/astar.hpp>

#include "pathfinder_settings.hpp"
#include "navigation_link.hpp"

namespace navigation {
using path_step =
    std::variant<Tile, std::shared_ptr<navigation_link>>;

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
        std::shared_ptr<navigation_link> link =
            std::get<std::shared_ptr<navigation_link>>(
                step);
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

    if (!_pfs.region_mapped(region, t.Plane))
      return {};

    constexpr std::array<
        std::tuple<int, int, Pathfinding::COLLISION_FLAG>,
        4>
        directions = {
            {{0, 1, Pathfinding::COLLISION_FLAG::NORTH},
             {1, 0, Pathfinding::COLLISION_FLAG::EAST},
             {0, -1, Pathfinding::COLLISION_FLAG::SOUTH},
             {-1, 0, Pathfinding::COLLISION_FLAG::WEST}}};

    auto flag = _pfs.get_collision(t);
    for (const auto [dx, dy, df] : directions) {
      if (flag & df)
        continue;

      const auto neighbor =
          Tile{t.X + dx, t.Y + dy, t.Plane};

      auto range =
          _pfs.navigation_links.equal_range(neighbor);
      for (auto it = range.first; it != range.second;
           ++it) {
        if (it->second->can_handle(_pfs))
          neighbors.emplace_back(it->second);
      }

      flag = _pfs.get_collision(neighbor);
      if (flag == Pathfinding::OPEN) {
        neighbors.emplace_back(neighbor);
      } else {
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
    } else if (auto nv = std::get_if<
                   std::shared_ptr<navigation_link>>(&t)) {
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

bool walk_path(const path &p) {
  auto at_end = [&]() {
    auto end = p.back();
    auto pos = Minimap::GetPosition();
    if (auto tile = std::get_if<Tile>(&end)) {
      return pos.DistanceFrom(*tile) < 4;
    } else if (auto nav = std::get_if<
                   std::shared_ptr<navigation_link>>(
                   &end)) {
      return pos.DistanceFrom((*nav)->to) < 4;
    }

    return false;
  };

  auto to_handle = [](path::const_iterator begin,
                      path::const_iterator end) {
    auto result = end;
    while (begin != end) {
      if (auto tile = std::get_if<Tile>(&*begin)) {
        if (Mainscreen::IsTileOn(*tile))
          result = begin;
      } else if (auto nav = std::get_if<
                     std::shared_ptr<navigation_link>>(
                     &*begin)) {
        auto pos = Minimap::GetPosition();
        if (pos.DistanceFrom((*nav)->from) <= 4) {
          result = begin;
        }
      }

      begin++;
    }

    return result;
  };

  auto handle_step = [](const path_step &step) {
    if (auto tile = std::get_if<Tile>(&step)) {
      if (!Mainscreen::IsTileOn(*tile)) {
        Camera::RotateTo(*tile, Camera::NORTH);
      }

      return Mainscreen::ClickTile(*tile);
    } else if (auto nv = std::get_if<
                   std::shared_ptr<navigation_link>>(
                   &step)) {
      return (*nv)->handle();
    }

    return false;
  };

  while (!at_end() && !Terminate) {
    auto handle = to_handle(p.begin(), p.end());
    auto dest = Minimap::GetDestination();
    if (!dest || !Mainscreen::IsMoving()) {
      handle_step(*handle);
    }
  }

  return true;
}

} // namespace pathfinding