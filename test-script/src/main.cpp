#include <Game/Core.hpp>

#include <astar/astar.hpp>
#include <pathfinding/gate_nv.hpp>
#include <pathfinding/pathfinding.hpp>

#include <chrono>

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

  pathfinding::navigation_link_map.emplace(
      Tile(2935, 3450, 0),
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

  auto path =
      pathfinding::find_path(location, Tile{3176, 3455, 0});
  
  // auto path =
  //     pathfinding::find_path(location, Tile{2944, 3452, 0});

  auto end = std::chrono::system_clock::now();

  auto took = end - now;

  std::cout << "took: "
            << std::chrono::duration_cast<
                   std::chrono::milliseconds>(took)
            << '\n';
  if (!path.has_value()) {
    std::cout << "No path found\n";
    return true;
  }

  // Paint::Clear();
  // for (const auto &path_step : path.value()) {
  //   // Paint::DrawTile(tile, 255, 0, 0, 255);
  //   if (auto tile = std::get_if<Tile>(&path_step)) {
  //     Paint::DrawTile(*tile, 255, 0, 0, 255);
  //   } else if (auto nav_link =
  //   std::get_if<std::shared_ptr<pathfinding::navigation_link>>(&path_step))
  //   {
  //     Paint::DrawTile((*nav_link)->from, 0, 255, 0, 255);
  //     Paint::DrawTile((*nav_link)->to, 0, 255, 0, 255);
  //   }
  // }

  // Paint::SwapBuffer();

  if (pathfinding::walk_path(path.value()))
    std::cout << "Success!!!\n";

  return true;
}

bool OnBreak() { return false; }

void OnEnd() {}