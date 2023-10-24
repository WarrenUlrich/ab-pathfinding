#include <Game/Core.hpp>

#include <astar/astar.hpp>

#include <pathfinding/navigation_link.hpp>
#include <pathfinding/pathfinding.hpp>
#include <pathfinding/region_plane.hpp>

namespace std {
template <> struct hash<Tile> {
  std::size_t operator()(const Tile &tile) const {
    std::size_t h1 = std::hash<std::int32_t>()(tile.X);
    std::size_t h2 = std::hash<std::int32_t>()(tile.Y);
    std::size_t h3 = std::hash<std::int32_t>()(tile.Plane);
    return h1 ^ (h2 << 1) ^ (h3 << 2);
  }
};

template <> struct hash<pathfinding::region_plane> {
  std::size_t
  operator()(const pathfinding::region_plane &rp) const {
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

void add_navigation_link(
    const std::shared_ptr<navigation_link> nvl) {
  navigation_link_map.emplace(nvl->to, nvl);
}

class tile_cost_heuristic_fn {
public:
  double operator()(const path_step &t1,
                    const path_step &t2) const {
    const auto to_tile = [](const path_step &pfn) {
      if (auto tile = std::get_if<Tile>(&pfn)) {
        return *tile;
      } else {
        return std::get<std::shared_ptr<navigation_link>>(
                   pfn)
            ->from;
      }
    };

    const auto tile1 = to_tile(t1);
    const auto tile2 = to_tile(t2);

    return tile1.DistanceFrom(tile2);
  }
};

class tile_neighbors_fn {
public:
  std::vector<path_step> operator()(const Tile &t) const {
    std::vector<path_step> neighbors;

    const auto region =
        ((std::int32_t)(((t.X >> 6) << 8) | (t.Y >> 6)));

    const auto rp = region_plane(region, t.Plane);

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

std::optional<path> find_path(const Tile &start,
                              const Tile &destination) {
  static auto pf = pathfinder{tile_cost_heuristic_fn{},
                              tile_cost_heuristic_fn{},
                              tile_neighbors_fn{}};

  return pf.search(start, destination);
}

bool walk_path(const path &path) {
  using path_iter_t = pathfinder::path::const_iterator;

  const auto on_screen = [](const Tile &tile) {
    const auto t2s =
        Internal::TileToMainscreen(tile, 0, 0, 0);
    if (!t2s)
      return false;

    const auto blocking_boxes =
        Mainscreen::GetBlockingWidgetBoxes();
    for (const auto &box : blocking_boxes) {
      if (box.Contains(t2s))
        return false;
    }

    const auto canvas = Internal::Client.GetCanvas();
    const auto box =
        Box(0, 0, canvas.GetWidth(), canvas.GetHeight());
    return box.Contains(t2s);
  };

  const auto get_next_step = [&](path_iter_t iter,
                                 path_iter_t end) {
    auto result = iter;

    const auto player = Players::GetLocal();
    if (!player)
      return result;

    for (; iter < end; ++iter) {
      if (const auto tile = std::get_if<Tile>(&*iter)) {
        if (!on_screen(*tile) ||
            tile->DistanceFrom(player.GetTile()) >= 15)
          continue;

        result = iter;
      } else if (const auto nv = std::get_if<
                     std::shared_ptr<navigation_link>>(
                     &*iter)) {
        auto to = (*nv)->to;
        if (to.DistanceFrom(player.GetTile()) <= 5) {
          result = iter;
        }

        break;
      }
    }

    return result;
  };

  const auto handle_tile = [](const Tile &tile) {
    if (!Mainscreen::ClickTile(tile)) {
      return false;
    }

    return WaitFunc(1000, 50, [&]() {
      return Minimap::GetDestination() == tile;
    });
  };

  auto path_iter = path.begin();
  const auto last_step = path.end() - 1;

  while (path_iter != last_step) {
    // Check if we're near the end.
    if (const auto last_tile =
            std::get_if<Tile>(&*last_step)) {
      const auto player = Players::GetLocal();
      if (last_tile->DistanceFrom(player.GetTile()) < 5) {
        return true;
      }
    }

    // Check and set camera pitch
    if (Camera::GetPitch() < 275) {
      Camera::SetPitch(UniformRandom(275, 325));
    }

    const auto next_step =
        get_next_step(path_iter, path.end());
    if (const auto tile = std::get_if<Tile>(&*next_step)) {
      if (!handle_tile(*tile))
        continue;

      path_iter = next_step;
      Wait(UniformRandom(500, 2000));
    } else if (const auto nv = std::get_if<
                   std::shared_ptr<navigation_link>>(
                   &*next_step)) {
      if (!(*nv)->handle())
        continue;

      path_iter = next_step + 1;
    }
  }

  return true;
}

} // namespace pathfinding