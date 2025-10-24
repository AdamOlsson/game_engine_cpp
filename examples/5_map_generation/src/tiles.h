#pragma once

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

inline std::ostream &operator<<(std::ostream &os,
                                std::tuple<CellType, CellType, CellType, CellType> type) {
    return os << "(" << std::get<0>(type) << ", " << std::get<1>(type) << ", "
              << std::get<2>(type) << ", " << std::get<3>(type) << ")";
}

struct ConstraintHashTmp {
    std::size_t
    operator()(const std::tuple<CellType, CellType, CellType, CellType> t) const {
        return (static_cast<size_t>(std::get<0>(t)) << 24) |
               (static_cast<size_t>(std::get<1>(t)) << 16) |
               (static_cast<size_t>(std::get<2>(t)) << 8) |
               (static_cast<size_t>(std::get<3>(t)));
    }
};
