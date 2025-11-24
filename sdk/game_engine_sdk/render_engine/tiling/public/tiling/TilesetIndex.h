#pragma once

#include <sstream>

namespace tiling {
struct TilesetIndex {
    unsigned int x;
    unsigned int y;
    TilesetIndex() = default;
    TilesetIndex(const unsigned int x, const unsigned int y) : x(x), y(y) {}

    std::string to_string() const {
        std::ostringstream oss;
        oss << "TilesetIndex(" << x << ", " << y << ")";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const TilesetIndex &obj) {
        return os << obj.to_string();
    }

    bool operator==(const TilesetIndex &other) const {
        return x == other.x && y == other.y;
    }
};

} // namespace tiling
