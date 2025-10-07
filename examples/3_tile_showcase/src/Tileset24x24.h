#pragma once

#include "game_engine_sdk/Grid.h"
#include "game_engine_sdk/render_engine/TilesetUVWT.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/GeometryPipeline.h"

namespace tileset24x24 {
inline std::vector<graphics_pipeline::GeometryInstanceBufferObject>
center_wall(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT &tileset_uvwt) {
    return {
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 3),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 0),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 2),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 1),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 1),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 3),
        },
    };
}
} // namespace tileset24x24

inline std::vector<graphics_pipeline::GeometryInstanceBufferObject>
get_showcase_24x24(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt) {
    std::vector<graphics_pipeline::GeometryInstanceBufferObject> tiles = {};

    auto t = tileset24x24::center_wall(x, y, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    return tiles;
}
