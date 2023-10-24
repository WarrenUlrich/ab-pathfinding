#include <Game/Core.hpp>

#include <astar/astar.hpp>

#include <pathfinding/bank.hpp>
#include <pathfinding/gate_nv.hpp>
#include <pathfinding/pathfinding.hpp>

#include <chrono>

#include <filesystem>

void Setup() {
  ScriptInfo Info;
  Info.Name = "ab-pathfinding-test";
  Info.Description = "";
  Info.Version = "1.00";
  Info.Category = "Magic";
  Info.Author = "Warren";
  Info.UID = "UID";
  Info.ForumPage = "forum.alpacabot.org";
  SetScriptInfo(Info);
}

bool OnStart() {
  const std::filesystem::path user_path =
      getenv("USERPROFILE");
  std::ifstream data(user_path / "AlpacaBot" /
                     "Collision Data" /
                     "collision_data.csv");

  pathfinding::load_collision_csv(data);

  pathfinding::add_navigation_link(
      std::make_shared<pathfinding::gate_nv>(
          Tile(2935, 3450, 0), Tile(2935, 3450, 0),
          Tile(2936, 3450, 0)));

  return true;
}

bool Loop() {
  auto player = Players::GetLocal();
  if (!player)
    return true;

  auto location = player.GetTile();
  auto now = std::chrono::system_clock::now();

  // auto closest_bank = pathfinding::bank::get_closest();

  auto &banks = pathfinding::bank::banks;

  auto closest = pathfinding::bank::get_closest();

  if (closest == banks.end())
    return true;

  auto bank_path = pathfinding::find_path(
      location, closest->get()->pathfinding_tile);

  const auto end = std::chrono::system_clock::now();

  std::cout << "took: "
            << std::chrono::duration_cast<
                   std::chrono::milliseconds>(end - now)
            << '\n';

  const auto draw_path = [](const auto &path) {
    for(auto idx = 0; idx < path.size() - 1; ++idx) {
      if (const auto tile = std::get_if<Tile>(&path[idx])) {
        const auto t2s = Internal::TileToMainscreen(*tile, 0, 0, 0);
      
        if (const auto next_tile = std::get_if<Tile>(&path[idx + 1])) {
          const auto next_t2s = Internal::TileToMainscreen(*next_tile, 0, 0, 0);
          Paint::DrawLine(t2s, next_t2s, 255, 0, 0, 255);
        }
      }
    }
  };

  Paint::Clear();
  draw_path(bank_path.value());
  Paint::SwapBuffer();

  // if (pathfinding::walk_path(path.value()))
  // std::cout << "Success!!!\n";

  return true;
}

bool OnBreak() { return false; }

void OnEnd() {}