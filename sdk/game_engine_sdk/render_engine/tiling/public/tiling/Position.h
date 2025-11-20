#pragma once

#include <cstddef>
#include <functional>
#include <sstream>

namespace tiling {
class Position {
  public:
    int x;
    int y;
    constexpr Position() = default;
    constexpr Position(int x, int y) : x(x), y(y) {};

    float manhattan_distance(const Position &other) const;
    float euclidian_distance(const Position &other) const;

    Position operator+(const Position &other) const;
    bool operator==(const Position &other) const;

    bool operator<(const Position &other) const;
    friend std::ostream &operator<<(std::ostream &os, const Position &obj) {
        return os << obj.to_string();
    }

    std::string to_string() const {
        std::ostringstream oss;
        oss << "Position(" << x << ", " << y << ")";
        return oss.str();
    }
};

struct PositionHash {
    size_t operator()(const tiling::Position &p) const {
        size_t h1 = std::hash<int>{}(p.x);
        size_t h2 = std::hash<int>{}(p.x);
        return h1 ^ (h2 << 1);
    }
};

} // namespace tiling
