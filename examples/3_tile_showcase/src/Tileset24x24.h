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

inline std::vector<graphics_pipeline::GeometryInstanceBufferObject>
left_wall(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT &tileset_uvwt) {
    return {
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 3),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(0, 0),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 2),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(0, 1),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 1),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(0, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(0, 3),
        },
    };
}

inline std::vector<graphics_pipeline::GeometryInstanceBufferObject>
right_wall(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT &tileset_uvwt) {
    return {
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 3),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(2, 0),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 2),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(2, 1),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 1),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(2, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(2, 3),
        },
    };
}

inline std::vector<graphics_pipeline::GeometryInstanceBufferObject>
door(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT &tileset_uvwt) {
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
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 1),
        },
    };
}

inline std::vector<graphics_pipeline::GeometryInstanceBufferObject>
jail_door(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT &tileset_uvwt) {
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
            .uvwt = tileset_uvwt.uvwt_for_tile_at(4, 1),
        },
    };
}

inline std::vector<graphics_pipeline::GeometryInstanceBufferObject>
torch(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT &tileset_uvwt) {
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
            .uvwt = tileset_uvwt.uvwt_for_tile_at(5, 1),
        },
    };
}

inline std::vector<graphics_pipeline::GeometryInstanceBufferObject>
floor(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT &tileset_uvwt) {
    return {
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 1),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 2),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 3),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 4),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 1, y),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 2, y),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 3, y),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 4, y),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 5, y),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 6, y),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 7, y),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 7, y + 1),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 7, y + 2),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 7, y + 3),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
        graphics_pipeline::GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 7, y + 4),
            .dimension = grid.cell_dimension(),
            .texture_idx = 1,
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2),
        },
    };
}
} // namespace tileset24x24

inline std::vector<graphics_pipeline::GeometryInstanceBufferObject>
get_showcase_24x24(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt) {
    std::vector<graphics_pipeline::GeometryInstanceBufferObject> tiles = {};

    auto t = tileset24x24::floor(x, y, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    t = tileset24x24::left_wall(x + 1, y + 1, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    t = tileset24x24::center_wall(x + 2, y + 1, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    t = tileset24x24::door(x + 3, y + 1, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    t = tileset24x24::torch(x + 4, y + 1, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    t = tileset24x24::jail_door(x + 5, y + 1, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    t = tileset24x24::right_wall(x + 6, y + 1, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    return tiles;
}
