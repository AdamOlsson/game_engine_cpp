#include "tiling/Position.h"
#include <cstdlib>

float tiling::Position::manhattan_distance(const tiling::Position &other) const {
    return static_cast<float>(abs(x - other.x) + abs(y - other.y));
}

float tiling::Position::euclidian_distance(const tiling::Position &other) const {
    const auto dx = abs(x - other.x);
    const auto dy = abs(y - other.y);
    // Note: There might be a bug here where I do not take the sqrt of the numbers.
    // I avoid this for efficiency
    return static_cast<float>(dx * dx + dy * dy);
}

tiling::Position tiling::Position::operator+(const tiling::Position &other) const {
    return tiling::Position(x + other.x, y + other.y);
}

bool tiling::Position::operator==(const tiling::Position &other) const {
    return x == other.x && y == other.y;
}

bool tiling::Position::operator<(const tiling::Position &other) const {
    if (x != other.x)
        return x < other.x;
    return y < other.y;
}
