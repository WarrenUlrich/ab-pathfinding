#include <Game/Core.hpp>

#include <pathfinding/pathfinding.hpp>
#include <astar/astar.hpp>

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
  const std::filesystem::path user_path = getenv("USERPROFILE");
  std::ifstream data(user_path / "AlpacaBot" / "Collision Data" / "collision_data.csv");
  ab::pathfinding::load_collision_csv(data);
  return true;
}

bool Loop() {
  auto player = Players::GetLocal();
  if (!player)
    return true;
  
  auto location = player.GetTile();
  auto now = std::chrono::system_clock::now();

  auto path = ab::pathfinding::find_path(location, Tile{3176, 3455, 0});

  auto end = std::chrono::system_clock::now();

  auto took = end - now;

  std::cout << "took: " << std::chrono::duration_cast<std::chrono::milliseconds>(took) << '\n';
  if (!path.has_value()) {
    std::cout << "No path found\n";
    return true;
  }

  Paint::Clear();
  for (const auto& tile : path.value()) {
    Paint::DrawTile(tile, 255, 0, 0, 255);
  }
  Paint::SwapBuffer();

  // Mainscreen::WalkPath(path.value(), 5);
  return true;
}

bool OnBreak() { return false; }

void OnEnd() {}