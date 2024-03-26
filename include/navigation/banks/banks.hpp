#pragma once

#include <memory>
#include <vector>

#include "bank_booth.hpp"

namespace navigation {
static std::vector<std::shared_ptr<bank>> banks = {
    // Varrock West - Northern booth
    std::make_shared<bank_booth>(Tile(3186, 3444, 0),
                                 Tile(3185, 3444, 0)),
    // Varrock West - Southern booth
    std::make_shared<bank_booth>(Tile(3186, 3436, 0),
                                 Tile(3185, 3436, 0)),
    // Varrock East - Middle booth
    std::make_shared<bank_booth>(Tile(3253, 3419, 0),
                                 Tile(3253, 3420, 0)),
    // Edgeville - NE booth
    std::make_shared<bank_booth>(Tile(3096, 3493, 0),
                                 Tile(3096, 3494, 0)),
};

std::shared_ptr<bank> get_closest_bank(const Tile &tile) {
  if (banks.empty()) {
    return nullptr;
  }

  auto closest_it = std::min_element(
      banks.begin(), banks.end(),
      [&tile](const std::shared_ptr<bank> &a,
              const std::shared_ptr<bank> &b) {
        return a->stand_tile.DistanceFrom(tile) <
               b->stand_tile.DistanceFrom(tile);
      });

  return *closest_it;
}
} // namespace navigation