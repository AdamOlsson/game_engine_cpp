#include "game_engine_sdk/Grid.h"

Grid::Grid(Dimension cell_size, WorldPoint center_offset)
    : m_cell_size(cell_size), m_center_offset(center_offset) {}

WorldPoint Grid::world_point_at(int x, int y) {
    return m_center_offset + WorldPoint(m_cell_size.x * x, m_cell_size.y * y);
}

Dimension Grid::cell_dimension() { return m_cell_size; }
