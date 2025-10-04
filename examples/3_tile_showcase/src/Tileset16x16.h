#pragma once

#include "game_engine_sdk/Grid.h"
#include "game_engine_sdk/render_engine/TilesetUVWT.h"
#include "game_engine_sdk/render_engine/graphics_pipeline/GeometryPipeline.h"

using namespace graphics_pipeline;
std::vector<GeometryInstanceBufferObject> left_wall(unsigned int x, unsigned int y,
                                                    Grid &grid, TilesetUVWT tileset_uvwt);
std::vector<GeometryInstanceBufferObject>
right_wall(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt);
std::vector<GeometryInstanceBufferObject>
center_wall(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt);
std::vector<GeometryInstanceBufferObject> door(unsigned int x, unsigned int y, Grid &grid,
                                               TilesetUVWT tileset_uvwt);
std::vector<GeometryInstanceBufferObject>
jail_bars_1(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt);
std::vector<GeometryInstanceBufferObject>
jail_bars_2(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt);
std::vector<GeometryInstanceBufferObject> torch(unsigned int x, unsigned int y,
                                                Grid &grid, TilesetUVWT tileset_uvwt);
std::vector<GeometryInstanceBufferObject> floor(unsigned int x, unsigned int y,
                                                Grid &grid, TilesetUVWT tileset_uvwt);

inline std::vector<GeometryInstanceBufferObject>
get_showcase_16x16(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt) {
    std::vector<GeometryInstanceBufferObject> tiles = {};
    auto t = floor(x, y, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    t = left_wall(x + 1, y + 1, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    t = center_wall(x + 2, y + 1, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    t = door(x + 3, y + 1, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    t = jail_bars_1(x + 4, y + 1, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    t = jail_bars_2(x + 5, y + 1, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    t = torch(x + 6, y + 1, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    t = right_wall(x + 7, y + 1, grid, tileset_uvwt);
    tiles.insert(tiles.begin(), t.begin(), t.end());

    return tiles;
}

inline std::vector<GeometryInstanceBufferObject>
torch(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt) {
    return {
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 2),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 0),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 1),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 1),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(6, 1),
        },
    };
}

inline std::vector<GeometryInstanceBufferObject>
jail_bars_1(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt) {
    return {
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 2),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 0),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 1),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 1),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(4, 1),
        },
    };
}

inline std::vector<GeometryInstanceBufferObject>
jail_bars_2(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt) {
    return {
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 2),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 0),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 1),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 1),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(5, 1),
        },
    };
}

inline std::vector<GeometryInstanceBufferObject>
door(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt) {
    return {
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 2),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 0),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 1),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 1),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(3, 1),
        },
    };
}

inline std::vector<GeometryInstanceBufferObject>
left_wall(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt) {
    return {
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 2),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(0, 0),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 1),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(0, 1),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(0, 2),
        },
    };
}

inline std::vector<GeometryInstanceBufferObject>
center_wall(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt) {
    return {
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 2),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 0),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 1),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 1),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(1, 2),
        },
    };
}

inline std::vector<GeometryInstanceBufferObject>
right_wall(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt) {
    return {
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 2),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(2, 0),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 1),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(2, 1),
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y),
            .dimension = grid.cell_dimension(),
            .uvwt = tileset_uvwt.uvwt_for_tile_at(2, 2),
        },
    };
}

inline std::vector<GeometryInstanceBufferObject>
floor(unsigned int x, unsigned int y, Grid &grid, TilesetUVWT tileset_uvwt) {
    auto floor_uvwt = tileset_uvwt.uvwt_for_tile_at(4, 2);
    auto floor_broken_uvwt = tileset_uvwt.uvwt_for_tile_at(3, 2);
    return {
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 3),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 2),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y + 1),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_broken_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x, y),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 1, y),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 2, y),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_broken_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 2, y),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 3, y),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 4, y),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_broken_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 5, y),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 6, y),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 7, y),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_uvwt,
        },

        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 8, y),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 8, y + 1),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 8, y + 2),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_uvwt,
        },
        GeometryInstanceBufferObject{
            .center = grid.world_point_at(x + 8, y + 3),
            .dimension = grid.cell_dimension(),
            .uvwt = floor_broken_uvwt,
        },
    };
}
