#pragma once

#include "tiling/TileGrid.h"
#include "tiling/traits.h"
#include "tiling/wang/TilesetConstraints.h"

namespace tiling::wang {

template <EnumUint8 TileType>
void assign_tile_sprites(TileGrid<TileType> &grid,
                         TilesetConstraints<TileType> &constraints) {

    for (auto i = 0; i < grid.width() * grid.height(); i++) {
        const int x = i % grid.width();
        const int y = i / grid.width();

        const int left = x - 1;
        const int right = x + 1;
        const int top = i - grid.width();
        const int bottom = i + grid.width();

        const TileType current_type = grid.get_cell(i).type;

        // For each cell, find the constraints based on bordering cells
        TileType left_constraint = static_cast<TileType>(0xFF);
        if (left >= 0) {
            left_constraint = grid.get_cell(i - 1).type;
        }

        TileType top_constraint = static_cast<TileType>(0xFF);
        if (top >= 0) {
            top_constraint = grid.get_cell(top).type;
        }

        TileType right_constraint = static_cast<TileType>(0xFF);
        if (right < grid.width()) {
            right_constraint = grid.get_cell(i + 1).type;
        }

        TileType bottom_constraint = static_cast<TileType>(0xFF);
        if (bottom < grid.width() * grid.height()) {
            bottom_constraint = grid.get_cell(bottom).type;
        }

        // TODO: A problem is that the outer most tiles will always have a
        // "None" constraint towards the edge of the map. Either I would need to
        // have a this "None" to a wildcard match or even easier, the outer most
        // tiles always have no texture.
        auto tileset_index =
            constraints.lookup_constraint(current_type, top_constraint, right_constraint,
                                          bottom_constraint, left_constraint);
        auto &tile = grid.get_cell(i);
        tile.sprite_index = tileset_index;
    }
}
} // namespace tiling::wang
