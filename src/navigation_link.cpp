#include <pathfinding/navigation_link.hpp>

namespace pathfinding {
navigation_link::navigation_link(const Tile &from,
                                 const Tile &to)
    : from(from), to(to) {}

navigation_link &
navigation_link::operator=(const navigation_link &other) {
  from = other.from;
  to = other.to;
  return *this;
}

navigation_link &
navigation_link::operator=(navigation_link &&other) {
  from = std::move(other.from);
  to = std::move(other.to);
  return *this;
}

bool navigation_link::operator==(
    const navigation_link &other) const {
  return from == other.from && to == other.to;
}

bool navigation_link::operator!=(
    const navigation_link &other) const {
  return !(*this == other);
}

bool navigation_link::can_handle() const { return true; }

} // namespace pathfinding
