#pragma once

#include <Game/Core.hpp>

#include <memory>

namespace pathfinding {
class bank {
public:
  std::string name;
  Area area;
  Tile pathfinding_tile;

  bank(const std::string &name, const Area &area,
       const Tile &pathfinding_tile)
      : name(std::move(name)), area(area),
        pathfinding_tile(pathfinding_tile) {}

  bool at() const {
    const auto player = Players::GetLocal();
    if (!player)
      return false;
    
    return area.Contains(player.GetTile());
  }

  virtual bool can_use() const {
    return true;
  }

  virtual bool open() const = 0;

  virtual ~bank() = default;

  static std::vector<std::unique_ptr<bank>> banks;

  static const bank* get_closest() {
    auto& closest = banks.front();
  
    const auto player =  Players::GetLocal();
    if (!player)
      return closest.get();

    const auto location = player.GetTile();
    std::int32_t dist = std::numeric_limits<std::int32_t>::max();

    for (const auto& bank : banks) {
      
    }

    return closest.get();
  }
};

// std::vector<bank> bank::banks = {
//     bank("Varrock West Bank",
//          Area(Tile(3180, 3446, 0), Tile(3185, 3443, 0)),
//          Tile(3182, 3441, 0))};

} // namespace pathfinding