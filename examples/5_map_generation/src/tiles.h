#pragma once

#include "tiling/wang/TilesetConstraints.h"
#include <cstdint>
#include <ostream>
enum class CellType : uint8_t {
    None,
    Wall,
    Grass,
};

inline std::ostream &operator<<(std::ostream &os, CellType type) {
    switch (type) {
    case CellType::None:
        return os << "CellType::None";
    case CellType::Wall:
        return os << "CellType::Wall";
    case CellType::Grass:
        return os << "CellType::Grass";
    default:
        return os << "Unknown";
    }
}

tiling::wang::TilesetConstraints<CellType> create_tileset_constraints();
