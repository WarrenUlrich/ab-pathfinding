#pragma once

#include <fstream>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <optional>

#include <Core/Types/Tile.hpp>

#include "bank.hpp"
#include "navigation_link.hpp"
#include "region_plane.hpp"

#include "navigation_links/navigation_links.hpp"

#include "pathfinder_settings.hpp"

namespace pathfinding {
extern std::unordered_set<region_plane> mapped_regions;

extern std::unordered_map<Tile, Pathfinding::COLLISION_FLAG>
    collision_map;

extern std::unordered_map<Tile,
                          std::shared_ptr<navigation_link>>
    navigation_link_map;

void load_collision_csv(std::ifstream &file);

void add_navigation_link(
    const std::shared_ptr<navigation_link> nvl);

using path_step =
    std::variant<Tile, std::shared_ptr<navigation_link>>;

using path = std::vector<path_step>;

std::optional<path> find_path(const Tile &start,
                              const Tile &dest);

bool walk_path(const path &p);

std::optional<Tile> find_walkable(const Tile &tile, std::int32_t dist);
} // namespace pathfinding