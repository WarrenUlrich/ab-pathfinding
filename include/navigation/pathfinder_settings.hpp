#pragma once

#include <fstream>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <Game/Core.hpp>

#include "navigation_link.hpp"
#include "region_plane.hpp"
#include "tile_hash.hpp"
#include "util.hpp"

namespace navigation {
class pathfinder_settings {
public:
  std::unordered_multimap<Tile,
                          std::shared_ptr<navigation_link>>
      navigation_links;

  pathfinder_settings() = default;

  void set_collision(const Tile &tile,
                     Pathfinding::COLLISION_FLAG flag) {
    _mapped_regions.emplace(
        region_plane(tile_to_region(tile), tile.Plane));
    _collision_map.emplace(tile, flag);
  }

  Pathfinding::COLLISION_FLAG
  get_collision(const Tile &tile) const {
    if (auto search = _collision_map.find(tile);
        search != _collision_map.end()) {
      return search->second;
    }

    return Pathfinding::OPEN;
  }

  void add_nav_link(
      std::shared_ptr<navigation_link> &&nav_link) {
    navigation_links.emplace(nav_link->from,
                             std::move(nav_link));
  }

  bool region_mapped(std::int32_t region_id,
                     std::int32_t plane = 0) const {
    return _mapped_regions.find(region_plane(
               region_id, plane)) != _mapped_regions.end();
  }

  bool tile_blocked(const Tile &tile) const {
    const auto found = _collision_map.find(tile);
    if (found == _collision_map.end())
      return false;

    return (found->second == Pathfinding::CLOSED) ||
           (found->second & Pathfinding::BLOCKED) ||
           (found->second & Pathfinding::OCCUPIED) ||
           (found->second & Pathfinding::SOLID);
  }

  void read_collision(std::ifstream &file) {
    std::string line;
    std::getline(file, line); // skip csv header
    while (std::getline(file, line)) {
      std::stringstream ss(line);
      std::string token;
      std::vector<std::string> tokens;
      while (std::getline(ss, token, ',')) {
        tokens.emplace_back(token);
      }

      // Adjusted to new CSV structure
      const auto x = std::stoi(tokens[0]);
      const auto y = std::stoi(tokens[1]);
      const auto plane = std::stoi(tokens[2]);
      const auto flag =
          static_cast<Pathfinding::COLLISION_FLAG>(
              std::stoi(tokens[3]));

      set_collision(Tile(x, y, plane), flag);
    }
  }

private:
  std::unordered_set<region_plane> _mapped_regions;

  std::unordered_map<Tile, Pathfinding::COLLISION_FLAG>
      _collision_map;
};
} // namespace pathfinding