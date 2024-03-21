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

  pathfinder_settings(const pathfinder_settings &other)
      : navigation_links(other.navigation_links),
        _mapped_regions(other._mapped_regions),
        _collision_map(other._collision_map) {}

  pathfinder_settings(pathfinder_settings &&other) noexcept
      : navigation_links(std::move(other.navigation_links)),
        _mapped_regions(std::move(other._mapped_regions)),
        _collision_map(std::move(other._collision_map)) {}
  
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

  std::optional<Tile>
  find_closest_walkable(const Tile &center_tile,
                        std::int32_t max_radius = 1) const {
    std::optional<Tile> closest_tile = Tile();
    double closest_distance =
        std::numeric_limits<double>::max();

    for (std::int32_t radius = 1; radius <= max_radius;
         ++radius) {
      bool found = false;
      for (std::int32_t dx = -radius; dx <= radius; ++dx) {
        for (std::int32_t dy = -radius; dy <= radius;
             ++dy) {
          if (std::abs(dx) != radius &&
              std::abs(dy) != radius)
            continue; // Skip tiles that are not on the
                      // current circle's perimeter

          Tile candidate_tile =
              Tile(center_tile.X + dx, center_tile.Y + dy,
                   center_tile.Plane);
          std::cout << "center: " << center_tile << '\n';
          if (!tile_blocked(candidate_tile)) {
            double distance = std::sqrt(dx * dx + dy * dy);
            if (distance < closest_distance) {
              closest_distance = distance;
              closest_tile = candidate_tile;
              found = true;
            }
          }
        }
      }
      if (found)
        break;
    }

    return closest_tile;
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
} // namespace navigation