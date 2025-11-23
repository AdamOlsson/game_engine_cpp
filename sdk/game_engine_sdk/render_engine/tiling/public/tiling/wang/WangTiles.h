#pragma once

#include "tiling/TileGrid.h"
#include "tiling/traits.h"
#include "tiling/wang/TilesetConstraints.h"

/// @namespace tiling::wang
/// Contains Wang tile constraints and lookup functionality for procedural tile-based
/// generation.
namespace tiling::wang {

/// @brief Looks up the appropriate sprite index for a tile based on Wang tile
/// constraints.
///
/// This function implements Wang tile constraint matching by examining the tile types of
/// neighboring cells and finding a compatible sprite from the tileset that satisfies all
/// edge constraints. Wang tiles are a procedural texture technique where tile
/// compatibility is determined by matching edge colors or types between adjacent tiles.
///
/// @tparam TileType An unsigned 8-bit enumeration type representing different tile
/// categories.
///
/// @param grid Reference to the tile grid to query for neighboring tile information.
/// @param constraints Reference to the constraint lookup table that stores valid tile
///                    combinations based on neighbor tile types.
/// @param x The column coordinate of the tile to look up.
/// @param y The row coordinate of the tile to look up.
///
/// @return An optional containing the sprite index if a valid sprite matching all
/// constraints
///         is found, or an empty optional if no compatible sprite exists.
///
/// @details
/// The function operates as follows:
/// 1. Retrieves the type of the current tile at (x, y).
/// 2. Queries the four neighboring cells (left, top, right, bottom) for their tile types.
/// 3. Treats out-of-bounds neighbors as having type 0xFF (a "None" constraint).
/// 4. Passes the current tile type and all four neighbor constraints to the lookup table
///    to find a sprite that satisfies all adjacency requirements.
///
/// @note **Known Issue**: Tiles at the edges of the map have a "None" constraint (0xFF)
///       towards the map boundary. This can cause mismatches if the constraint system
///       doesn't account for boundary tiles. Potential solutions include:
///       - Treating "None" as a wildcard that matches any neighbor type
///       - Restricting outer-most tiles to have no texture (treated specially)
///       - Defining explicit boundary tile types
///
/// @see TilesetConstraints for constraint lookup implementation
/// @see TileGrid for tile storage and access
template <EnumUint8 TileType>
std::optional<TilesetIndex>
lookup_tile_sprite(TileGrid<TileType> &grid,
                   const TilesetConstraints<TileType> &constraints, const int x,
                   const int y) {

    const TileType current_type = grid.get_cell(x, y).type;

    // For each cell, find the constraints based on bordering cells
    TileType left_constraint = static_cast<TileType>(0xFF);
    if (x - 1 >= 0) {
        left_constraint = grid.get_cell(x - 1, y).type;
    }

    TileType top_constraint = static_cast<TileType>(0xFF);
    if (y - 1 >= 0) {
        top_constraint = grid.get_cell(x, y - 1).type;
    }

    TileType right_constraint = static_cast<TileType>(0xFF);
    if (x + 1 < grid.width()) {
        right_constraint = grid.get_cell(x + 1, y).type;
    }

    TileType bottom_constraint = static_cast<TileType>(0xFF);
    if (y + 1 < grid.width() * grid.height()) {
        bottom_constraint = grid.get_cell(x, y + 1).type;
    }

    // TODO: A problem is that the outer most tiles will always have a
    // "None" constraint towards the edge of the map. Either I would need to
    // have a this "None" to a wildcard match or even easier, the outer most
    // tiles always have no texture.
    return constraints.lookup_constraint(current_type, top_constraint, right_constraint,
                                         bottom_constraint, left_constraint);
}
} // namespace tiling::wang
