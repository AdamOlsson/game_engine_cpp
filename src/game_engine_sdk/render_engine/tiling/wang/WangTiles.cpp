#include "game_engine_sdk/render_engine/tiling/wang/WangTiles.h"

tiling::WangTiles::WangTiles(
    const tiling::NoiseMap &noise_map, std::function<CellType(float)> tile_assign_rule,
    std::unordered_map<std::tuple<tiling::CellType, tiling::CellType, tiling::CellType,
                                  tiling::CellType>,
                       glm::vec4, tiling::ConstraintHash> &&tileset_constraints)
    : m_grid_width(noise_map.width), m_grid_height(noise_map.height),
      m_tileset_constraints(std::move(tileset_constraints)) {
    m_cells = assign_cell_types(std::move(noise_map), std::move(tile_assign_rule));
    m_cell_sprites = assign_cell_sprites(m_cells);
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

std::vector<glm::vec4>
tiling::WangTiles::assign_cell_sprites(std::vector<CellType> &cell_types) {
    std::vector<glm::vec4> cell_sprites;
    cell_sprites.reserve(m_grid_width * m_grid_height);
    for (auto i = 0; i < cell_types.size(); i++) {
        const int x = i % m_grid_width;
        const int y = i / m_grid_width;

        const int current = i;
        const int left = x - 1;
        const int right = x + 1;
        const int top = current - m_grid_width;
        const int bottom = current + m_grid_width;

        // For each cell, find the constraints based on bordering cells
        CellType left_constraint = CellType::None;
        if (left >= 0) {
            left_constraint = cell_types[i - 1];
        }

        CellType top_constraint = CellType::None;
        if (top >= 0) {
            top_constraint = cell_types[top];
        }

        CellType right_constraint = CellType::None;
        if (right < m_grid_width) {
            right_constraint = cell_types[i + 1];
        }

        CellType bottom_constraint = CellType::None;
        if (bottom < m_grid_width * m_grid_height) {
            bottom_constraint = cell_types[bottom];
        }

        const std::tuple<CellType, CellType, CellType, CellType> constraints = std::tuple(
            top_constraint, right_constraint, bottom_constraint, left_constraint);

        // TODO: A problem is that the outer most tiles will always have a "None"
        // constraint towards the edge of the map. Either I would need to have a
        // this "None" to a wildcard match or even easier, the outer most tiles
        // always have no texture.
        /*logger::debug(i, ": looking for constraint ", constraints);*/
        if (m_tileset_constraints.find(constraints) != m_tileset_constraints.end()) {
            cell_sprites.push_back(m_tileset_constraints[constraints]);

        } else {
            // TODO: Create some wrapper around uvwt coordinates have this be "no
            // uvwt"
            cell_sprites.push_back(glm::vec4(-1.0f));
        }
    }
    return cell_sprites;
}

unsigned int tiling::WangTiles::width() { return m_grid_width; }
unsigned int tiling::WangTiles::height() { return m_grid_height; }

std::vector<tiling::CellType> tiling::WangTiles::get_cells() { return m_cells; }
std::vector<glm::vec4> tiling::WangTiles::get_cell_uvwt() { return m_cell_sprites; }

void tiling::WangTiles::print_cell_type() {
    for (auto i = 0; i < m_cells.size(); i++) {
        std::cout << m_cells[i] << " ";
        if ((i + 1) % m_grid_width == 0) {
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
}
