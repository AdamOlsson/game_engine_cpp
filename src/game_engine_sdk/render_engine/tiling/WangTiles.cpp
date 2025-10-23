#include "game_engine_sdk/render_engine/tiling/WangTiles.h"

tiling::WangTiles::WangTiles(const tiling::NoiseMap &noise_map,
                             std::function<CellType(float)> tile_assign_rule)
    : m_grid_width(noise_map.width), m_grid_height(noise_map.height) {
    m_cells = assign_cell_types(std::move(noise_map), std::move(tile_assign_rule));
}

std::vector<tiling::CellType>
tiling::WangTiles::assign_cell_types(const NoiseMap &noise_map,
                                     std::function<CellType(float)> tile_assign_rule) {
    std::vector<CellType> cell_types;
    cell_types.reserve(noise_map.size());
    for (auto c : noise_map.noise) {
        cell_types.push_back(tile_assign_rule(c));
    }
    return cell_types;
}

std::vector<tiling::CellType> tiling::WangTiles::get_cells() { return m_cells; }

void tiling::WangTiles::print_cell_type() {
    for (auto i = 0; i < m_cells.size(); i++) {
        std::cout << m_cells[i] << " ";
        if ((i + 1) % m_grid_width == 0) {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}
