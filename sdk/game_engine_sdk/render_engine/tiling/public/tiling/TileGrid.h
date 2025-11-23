#pragma once

#include "tiling/Grid.h"
namespace tiling {

template <typename TileType> struct Tile {
    TileType type;
    float weight = 1.0f;
};

template <typename TileType> using TileGrid = Grid<Tile<TileType>>;

} // namespace tiling
