#pragma once

#include <cstddef>
#include <functional>
namespace tiling {
class Position {
  public:
    int x;
    int y;
    constexpr Position() = default;
    constexpr Position(int x, int y) : x(x), y(y) {};

    Position operator+(const Position &other) const;
    bool operator==(const Position &other) const;
    bool operator<(const Position &other) const;

    float manhattan_distance(const Position &other) const;
    float euclidian_distance(const Position &other) const;
};

struct PositionHash {
    size_t operator()(const tiling::Position &p) const {
        size_t h1 = std::hash<int>{}(p.x);
        size_t h2 = std::hash<int>{}(p.x);
        return h1 ^ (h2 << 1);
    }
};

} // namespace tiling
