#include <pathfinding/banks/banker_bank.hpp>

#include <Game/Core.hpp>

namespace pathfinding {
banker_bank::banker_bank(const std::string &name,
                         const Area &area,
                         const Tile &pathfinding_tile)
    : bank(name, area, pathfinding_tile) {}

bool banker_bank::open() const {
  const auto banker = NPCs::Get("Banker");
  if (!banker)
    return false;
  
  if (!banker.Interact("Bank Banker"))
    return false;
  
  return WaitFunc(1000, 50, Bank::IsOpen);
}
} // namespace pathfinding