#include <pathfinding/bank.hpp>
#include <pathfinding/banks/bank_booth_bank.hpp>
#include <pathfinding/banks/banker_bank.hpp>

#include <Game/Core.hpp>

namespace pathfinding {
bank::bank(const bank &other)
    : name(other.name), area(other.area),
      pathfinding_tile(other.pathfinding_tile) {}

bank::bank(const std::string &name, const Area &area,
           const Tile &pathfinding_tile)
    : name(std::move(name)), area(area),
      pathfinding_tile(pathfinding_tile) {}

bool bank::at() const {
  const auto player = Players::GetLocal();
  if (!player)
    return false;

  return area.Contains(player.GetTile());
}

bool bank::can_use() const { return true; }

// clang-format off
const bank::bank_vec bank::banks = {
  std::make_shared<bank_booth_bank>(
    "Varrock West Bank",
    Area(Tile(3180, 3446, 0), Tile(3185, 3443, 0)),
    Tile(3182, 3441, 0)
  ),
  std::make_shared<banker_bank>(
    "Grand Exchange",
    Area(Tile(3158, 3496, 0), Tile(3171, 3482, 0)),
    Tile(3164, 3485, 0)
  )
};
// clang-format on

bank::bank_vec::const_iterator bank::get_closest() {
  const auto player = Players::GetLocal();
  if (!player)
    return banks.end();

  const auto location = player.GetTile();
  return std::max_element(
      banks.begin(), banks.end(), [&](const auto &a, const auto &b) {
        const auto a_dist =
            location.DistanceFrom(a->pathfinding_tile);
        const auto b_dist =
            location.DistanceFrom(b->pathfinding_tile);
        return a_dist > b_dist;
      });
}

} // namespace pathfinding