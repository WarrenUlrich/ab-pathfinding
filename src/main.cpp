#include <Game/Core.hpp>

void Setup() {
  ScriptInfo Info;
  Info.Name = "ab-pathfinding";
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
  return true;
}

bool OnBreak() { return false; }

void OnEnd() {}