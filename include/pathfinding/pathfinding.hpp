#pragma once

#include <fstream>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "navigation_link.hpp"
#include "region_plane.hpp"

class Tile;

namespace std {
template <> struct hash<Tile>;
template <> struct hash<pathfinding::region_plane>;
template <> struct hash<pathfinding::navigation_link>;
} // namespace std

namespace pathfinding {
extern std::unordered_set<region_plane> mapped_regions;

extern std::unordered_map<Tile, Pathfinding::COLLISION_FLAG>
    collision_map;

extern std::unordered_map<Tile,
                          std::shared_ptr<navigation_link>>
    navigation_link_map;

void load_collision_csv(std::ifstream &file);

using path_step =
    std::variant<Tile, std::shared_ptr<navigation_link>>;

using path = std::vector<path_step>;

std::optional<path> find_path(const Tile &start,
                              const Tile &dest);

bool walk_path(const path &p);
} // namespace pathfinding