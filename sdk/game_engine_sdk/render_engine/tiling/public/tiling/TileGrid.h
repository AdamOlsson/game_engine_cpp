#pragma once

#include "tiling/Grid.h"
#include "tiling/TilesetIndex.h"
namespace tiling {

template <typename TileType> struct Tile {
    TileType type;
    std::optional<TilesetIndex> sprite_index = std::nullopt;
    float weight = 1.0f;
};

template <typename TileType> using TileGrid = Grid<Tile<TileType>>;

} // namespace tiling
