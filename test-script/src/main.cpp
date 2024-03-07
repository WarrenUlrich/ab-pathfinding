#include <Game/Core.hpp>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <pathfinding/pathfinding.hpp>
#include <pathfinding/navigation_links/relekka_to_ungael.hpp>
#include <pathfinding/navigation_links/ungael_to_relekka.hpp>
#include <variant>

void Setup() {
  ScriptInfo Info;
  Info.Name = "vorkath";
  Info.Description = "";
  Info.Version = "1.00";
  Info.Category = "Combat";
  Info.Author = "Warren";
  Info.UID = "UID";
  Info.ForumPage = "forum.alpacabot.org";
  SetScriptInfo(Info);
}

pathfinding::pathfinder_settings pfs;

bool OnStart() {
   const std::filesystem::path user_path =
      getenv("USERPROFILE");
  std::ifstream data(user_path / "AlpacaBot" /
                     "Collision Data" /
                     "collision_data.csv");

  pfs.read_collision(data);
  pfs.add_nav_link(std::make_shared<pathfinding::relekka_to_ungael>());
  pfs.add_nav_link(std::make_shared<pathfinding::ungael_to_relekka>());
  SetLoopDelay(5);
  return true;
}

bool Loop() {
  // agent.decide_and_act(state);
  Paint::Clear();
  const auto pos = Minimap::GetPosition();
  const auto tile = Tile(2272, 4044, 0);
  // const auto tile = Tile(3163, 3484, 0);
  auto path = pathfinding::find_path(pos, tile, pfs);
  if (!path.has_value()) {
    std::cout << "No path found\n";
    return true;
  }

  // for (const auto &step : path.value()) {
  //   if (auto tile = std::get_if<Tile>(&step)) {
  //     Paint::DrawTile(*tile, 255, 0, 0, 255);
  //   }
  // }
  pathfinding::walk_path(path.value());
  Paint::SwapBuffer();
  return true;
}

bool OnBreak() { return false; }
void OnEnd() {}
