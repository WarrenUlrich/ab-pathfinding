#pragma once

#include <Game/Core.hpp>

#include <astar/astar.hpp>

#include <array>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "bank.hpp"
#include "navigation_link.hpp"

class region_plane { // TODO: Put this somewhere cleaner
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

namespace std {
template <> struct hash<Tile> {
  std::size_t operator()(const Tile &tile) const {
    std::size_t h1 = std::hash<std::int32_t>()(tile.X);
    std::size_t h2 = std::hash<std::int32_t>()(tile.Y);
    std::size_t h3 = std::hash<std::int32_t>()(tile.Plane);
    return h1 ^ (h2 << 1) ^ (h3 << 2);
  }
};

template <> struct hash<region_plane> {
  std::size_t operator()(const region_plane &rp) const {
    std::size_t h1 = std::hash<std::int32_t>()(rp.region);
    std::size_t h2 = std::hash<std::int32_t>()(rp.plane);
    return h1 ^ (h2 << 1);
  }
};

template <> struct hash<pathfinding::navigation_link> {
  std::size_t
  operator()(const pathfinding::navigation_link &nv) const {
    std::size_t h1 = std::hash<Tile>()(nv.from);
    std::size_t h2 = std::hash<Tile>()(nv.to);
    return h1 ^ (h2 << 1);
  }
};
} // namespace std

namespace pathfinding {
std::unordered_set<region_plane> mapped_regions;

std::unordered_map<Tile, Pathfinding::COLLISION_FLAG>
    collision_map;

std::unordered_map<Tile, std::shared_ptr<navigation_link>>
    navigation_link_map;

void load_collision_csv(std::ifstream &file) {
  std::string line;
  std::getline(file, line); // skip csv header
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;
    while (std::getline(ss, token, ',')) {
      tokens.emplace_back(token);
    }

    const auto region = std::stoi(tokens[0]);
    const auto plane = std::stoi(tokens[1]);
    const auto x = std::stoi(tokens[2]);
    const auto y = std::stoi(tokens[3]);
    const auto flag =
        static_cast<Pathfinding::COLLISION_FLAG>(
            std::stoi(tokens[4]));

    mapped_regions.emplace(region, plane);
    collision_map.emplace(Tile(x, y, plane), flag);
  }
}

using path_step =
    std::variant<Tile, std::shared_ptr<navigation_link>>;

class tile_cost_heuristic_fn {
public:
  double operator()(const path_step &t1,
                    const path_step &t2) const {
    auto to_tile = [](const path_step &pfn) {
      if (auto tile = std::get_if<Tile>(&pfn)) {
        return *tile;
      } else {
        return std::get<std::shared_ptr<navigation_link>>(
                   pfn)
            ->from;
      }
    };

    auto tile1 = to_tile(t1);
    auto tile2 = to_tile(t2);

    return tile1.DistanceFrom(tile2);
  }
};

class tile_neighbors_fn {
public:
  std::vector<path_step> operator()(const Tile &t) const {
    std::vector<path_step> neighbors;

    auto region =
        ((std::int32_t)(((t.X >> 6) << 8) | (t.Y >> 6)));

    auto rp = region_plane(region, t.Plane);

    if (mapped_regions.find(rp) == mapped_regions.end())
      return {};

    constexpr std::array<
        std::tuple<int, int, Pathfinding::COLLISION_FLAG>,
        4>
        directions = {
            {{0, 1, Pathfinding::COLLISION_FLAG::NORTH},
             {1, 0, Pathfinding::COLLISION_FLAG::EAST},
             {0, -1, Pathfinding::COLLISION_FLAG::SOUTH},
             {-1, 0, Pathfinding::COLLISION_FLAG::WEST}}};

    auto flag = Pathfinding::COLLISION_FLAG::OPEN;
    if (auto search = collision_map.find(t);
        search != collision_map.end()) {
      flag = search->second;
    }

    for (const auto [dx, dy, df] : directions) {
      if (flag & df)
        continue;

      const auto neighbor =
          Tile{t.X + dx, t.Y + dy, t.Plane};

      if (auto search = navigation_link_map.find(neighbor);
          search != navigation_link_map.end()) {
        neighbors.emplace_back(search->second);
      }

      auto search = collision_map.find(neighbor);
      if (search == collision_map.end()) {
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
    } else if (auto nv = std::get_if<
                   std::shared_ptr<navigation_link>>(&t)) {
      return this->operator()(*nv);
    }

    return {};
  }
};

using pathfinder =
    astar::pathfinder<path_step, tile_cost_heuristic_fn,
                      tile_cost_heuristic_fn,
                      tile_neighbors_fn>;

auto find_path(const Tile &start, const Tile &destination) {
  static auto pf = pathfinder{tile_cost_heuristic_fn{},
                              tile_cost_heuristic_fn{},
                              tile_neighbors_fn{}};

  return pf.search(start, destination);
}
} // namespace pathfinding