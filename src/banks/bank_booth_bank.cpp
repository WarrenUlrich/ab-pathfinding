#include <pathfinding/banks/bank_booth_bank.hpp>

#include <Game/Core.hpp>

namespace pathfinding {
bank_booth_bank::bank_booth_bank(
    const std::string &name, const Area &area,
    const Tile &pathfinding_tile)
    : bank(name, area, pathfinding_tile) {}

bool bank_booth_bank::open() const {
  const auto bank_booth = GameObjects::Get("Bank booth");
  if (!bank_booth)
    return false;

  if (!bank_booth.Interact("Bank Bank booth"))
    return false;

  return WaitFunc(1000, 50, Bank::IsOpen);
}
} // namespace pathfinding