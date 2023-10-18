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
  return true;
}

bool Loop() {
  auto player = Players::GetLocal();
  if (!player)
    return true;
  
  auto location = player.GetTile();
  auto now = std::chrono::system_clock::now();

  auto path = ab::pathfinding::find_path(location, Tile{
    location.X + 10,
    location.Y + 10,
    location.Plane
  });

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
  return true;
}

bool OnBreak() { return false; }

void OnEnd() {}