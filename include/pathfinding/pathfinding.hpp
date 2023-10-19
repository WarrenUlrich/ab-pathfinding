#pragma once

#include <Game/Core.hpp>

#include <astar/astar.hpp>

#include <array>
#include <filesystem>
#include <fstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "bank.hpp"

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
} // namespace std

namespace ab::pathfinding {
std::unordered_set<region_plane> mapped_regions;

std::unordered_map<Tile, Pathfinding::COLLISION_FLAG>
    collision_map;

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