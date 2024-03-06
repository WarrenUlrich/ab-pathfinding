#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_set>

#include <Game/Core.hpp>

void Setup() {
  ScriptInfo Info;
  Info.Name = "collision-cacher";
  Info.Description =
      "Caches collision data around the player.";
  Info.Version = "1.00";
  Info.Category = "Magic";
  Info.Author = "Warren";
  Info.UID = "UID";
  Info.ForumPage = "forum.alpacabot.org";
  SetScriptInfo(Info);
}

class region_key {
public:
  std::int32_t region;
  std::int32_t plane;

  region_key() = default;

  region_key(std::int32_t region, std::int32_t plane)
      : region(region), plane(plane) {}

  bool operator==(const region_key &other) const {
    return region == other.region && plane == other.plane;
  }
};

class collision {
public:
  Tile tile;
  Pathfinding::COLLISION_FLAG flag;

  collision(const Tile &tile,
            Pathfinding::COLLISION_FLAG flag)
      : tile(tile), flag(flag) {}

  bool operator==(const collision &other) const {
    return tile == other.tile && flag == other.flag;
  }
};

namespace std {
template <> struct hash<region_key> {
  std::size_t operator()(const region_key &key) const {
    std::size_t h1 = std::hash<std::int32_t>()(key.region);
    std::size_t h2 = std::hash<std::int32_t>()(key.plane);
    return h1 ^ (h2 << 1);
  }
};

template <> struct hash<Tile> {
  std::size_t operator()(const Tile &tile) const {
    std::size_t h1 = std::hash<std::int32_t>()(tile.X);
    std::size_t h2 = std::hash<std::int32_t>()(tile.Y);
    std::size_t h3 = std::hash<std::int32_t>()(tile.Plane);
    return h1 ^ (h2 << 1) ^ (h3 << 2);
  }
};

template <> struct hash<collision> {
  std::size_t operator()(const collision &collision) const {
    std::size_t h1 = std::hash<Tile>()(collision.tile);
    std::size_t h2 = std::hash<std::int32_t>()(
        static_cast<std::int32_t>(collision.flag));
    return h1 ^ (h2 << 1);
  }
};
} // namespace std

std::unordered_map<region_key,
                   std::unordered_set<collision>>
    cached_regions;

std::int32_t get_tile_region(const Tile &tile) {
  return (
      (std::int32_t)(((tile.X >> 6) << 8) | (tile.Y >> 6)));
}

void cache_current_region() {
  const auto to_world_tile =
      [](const Pathfinding::TileNode &node,
         std::int32_t client_x, std::int32_t client_y,
         std::int32_t client_plane) {
        return Tile(node.X + client_x, node.Y + client_y,
                    client_plane);
      };

  Pathfinding::GenerateNodes();
  const std::int32_t client_x = Internal::GetClientX();
  const std::int32_t client_y = Internal::GetClientY();
  const std::int32_t client_plane =
      Internal::GetClientPlane();

  for (const auto &nodes : Pathfinding::GetNodes()) {
    for (const auto &node : nodes) {
      const auto flag = node.Flag;
      const auto world_tile = to_world_tile(
          node, client_x, client_y, client_plane);

      if ((flag & Pathfinding::UNINITIALIZED) ||
          flag == Pathfinding::UNINITIALIZED) {
        continue;
      }

      if ((flag == Pathfinding::CLOSED)) {
        continue;
      }

      std::int32_t current_region =
          get_tile_region(world_tile);

      cached_regions[region_key(current_region,
                                client_plane)]
          .emplace(world_tile,
                   static_cast<Pathfinding::COLLISION_FLAG>(
                       flag));
    }
  }
}

void paint() {
  Paint::Clear();
  std::vector<std::string> texts;
  auto local_tile = Players::GetLocal().GetTile();
  for (const auto &[key, collision_data] : cached_regions) {
    texts.emplace_back(
        "(" + std::to_string(key.region) + ", " +
        std::to_string(key.plane) +
        "): " + std::to_string(collision_data.size()));

    for (const auto &collision : collision_data) {
      if (collision.tile.DistanceFrom(local_tile) > 12)
        continue;

      if ((collision.flag & Pathfinding::BLOCKED) ||
          (collision.flag & Pathfinding::OCCUPIED)) {
        Paint::DrawTile(collision.tile, 255, 0, 0, 255);
      } else if (collision.flag & Pathfinding::NORTH) {
        Paint::DrawTile(collision.tile, 255, 0, 255, 255);
      } else if (collision.flag & Pathfinding::EAST) {
        Paint::DrawTile(collision.tile, 0, 255, 0, 255);
      } else if (collision.flag & Pathfinding::SOUTH) {
        Paint::DrawTile(collision.tile, 0, 0, 255, 255);
      } else if (collision.flag & Pathfinding::WEST) {
        Paint::DrawTile(collision.tile, 255, 165, 0, 255);
      }
    }
  }

  Point p = Point(0, 5);
  for (const auto &text : texts) {
    p += Point(0, 20);
    Paint::DrawString(text, p, 0, 255, 0, 255);
  }
  Paint::SwapBuffer();
}

void verify_and_clean() {
  // verify each tile in a region actually belongs to it, if
  // not, remove it
  for (auto it = cached_regions.begin();
       it != cached_regions.end();) {
    const auto &[key, collision_data] = *it;
    const auto get_tile_region = [](const Tile &tile) {
      return ((std::int32_t)(((tile.X >> 6) << 8) |
                             (tile.Y >> 6)));
    };

    for (const auto &collision : collision_data) {
      if (get_tile_region(collision.tile) != key.region) {
        it->second.erase(collision);
      }
    }

    if (it->second.empty()) {
      it = cached_regions.erase(it);
    } else {
      ++it;
    }
  }
}

void write_collision_data(std::ofstream &file) {
  // Updated CSV header
  file << "x,y,plane,flag\n";
  verify_and_clean();
  for (const auto &[key, collision_data] : cached_regions) {
    for (const auto &collision : collision_data) {
      if (collision.flag == Pathfinding::OPEN)
        continue;

      // Exclude key.region from output
      file << collision.tile.X << "," << collision.tile.Y
           << "," << key.plane << ","
           << static_cast<std::int32_t>(collision.flag)
           << "\n";
    }
  }
}

void read_collision_data(std::ifstream &file) {
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
    // Corrected to match the CSV structure
    const auto x = std::stoi(tokens[0]);
    const auto y = std::stoi(tokens[1]);
    const auto plane = std::stoi(tokens[2]);
    const auto flag = static_cast<Pathfinding::COLLISION_FLAG>(std::stoi(tokens[3]));

    // Create or update the collision data without using
    // region
    cached_regions[region_key(
                       get_tile_region(Tile(x, y, plane)),
                       plane)]
        .emplace(Tile(x, y, plane), flag);
  }
}

std::string data_url() {
  char *userprofile = std::getenv("USERPROFILE");
  if (userprofile == nullptr)
    return "";

  return std::string(userprofile) +
         "\\AlpacaBot\\Collision Data\\collision_data.csv";
}

bool OnStart() {
  SetLoopDelay(5);

  std::string path = data_url();
  std::ifstream file(path);
  read_collision_data(file);
  return true;
}

bool Loop() {
  cache_current_region();
  paint();
  return true;
}

bool OnBreak() { return true; }

void OnEnd() {
  char *userprofile = std::getenv("USERPROFILE");
  if (userprofile == nullptr)
    return;

  std::string path = data_url();
  if (!std::filesystem::exists(path)) {
    std::filesystem::create_directories(
        std::string(userprofile) +
        "\\AlpacaBot\\Collision Data\\");
  }

  std::ofstream file(path);
  write_collision_data(file);
}