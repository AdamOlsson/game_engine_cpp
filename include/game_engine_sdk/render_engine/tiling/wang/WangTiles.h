#pragma once

#include "game_engine_sdk/io.h"
#include "game_engine_sdk/render_engine/tiling/NoiseMap.h"
#include "glm/glm.hpp"
#include <vector>

namespace tiling {
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

struct ConstraintHash {
    std::size_t
    operator()(const std::tuple<CellType, CellType, CellType, CellType> t) const {
        return (static_cast<size_t>(std::get<0>(t)) << 24) |
               (static_cast<size_t>(std::get<1>(t)) << 16) |
               (static_cast<size_t>(std::get<2>(t)) << 8) |
               (static_cast<size_t>(std::get<3>(t)));
    }
};

class WangTiles {
  private:
    unsigned int m_grid_width;
    unsigned int m_grid_height;
    std::vector<CellType> m_cells;

    std::unordered_map<std::tuple<tiling::CellType, tiling::CellType, tiling::CellType,
                                  tiling::CellType>,
                       glm::vec4, tiling::ConstraintHash>
        m_tileset_constraints;

    // TODO: glm::vec4 in a UVWT class?
    std::vector<glm::vec4> m_cell_sprites; // uvwt

    std::vector<CellType>
    assign_cell_types(const tiling::NoiseMap &noise_map,
                      std::function<CellType(float)> tile_assign_rule);

    std::vector<glm::vec4> assign_cell_sprites(std::vector<CellType> &cell_types);

  public:
    WangTiles() = default;
    ~WangTiles() = default;
    WangTiles(
        const tiling::NoiseMap &noise_map,
        std::function<CellType(float)> tile_assign_rule,
        std::unordered_map<std::tuple<tiling::CellType, tiling::CellType,
                                      tiling::CellType, tiling::CellType>,
                           glm::vec4, tiling::ConstraintHash> &&tileset_constraints);

    unsigned int width();
    unsigned int height();

    void print_cell_type();

    // Temporary?
    std::vector<CellType> get_cells();
    std::vector<glm::vec4> get_cell_uvwt();
};
} // namespace tiling
