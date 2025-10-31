#include "tiles.h"

tiling::wang::TilesetConstraints<CellType> create_tileset_constraints() {
    auto tileset_constraints = tiling::wang::TilesetConstraints<CellType>();
    tileset_constraints.add_constraint(
        tiling::wang::TilesetIndex(1, 0),
        tiling::wang::TilesetTile<CellType>{.type = CellType::Wall,
                                            .constraints = {
                                                .north = {CellType::Wall},
                                                .east = {CellType::Wall},
                                                .south = {CellType::Wall},
                                                .west = {CellType::Wall},
                                            }});

    tileset_constraints.add_constraint(
        tiling::wang::TilesetIndex(2, 0),
        tiling::wang::TilesetTile<CellType>{.type = CellType::Wall,
                                            .constraints = {
                                                .north = {CellType::Grass},
                                                .east = {CellType::Grass},
                                                .south = {CellType::Wall},
                                                .west = {CellType::Grass},
                                            }});

    tileset_constraints.add_constraint(
        tiling::wang::TilesetIndex(1, 1),
        tiling::wang::TilesetTile<CellType>{.type = CellType::Wall,
                                            .constraints = {
                                                .north = {CellType::Grass},
                                                .east = {CellType::Wall},
                                                .south = {CellType::Wall},
                                                .west = {CellType::Grass},
                                            }});

    tileset_constraints.add_constraint(
        tiling::wang::TilesetIndex(2, 1),
        tiling::wang::TilesetTile<CellType>{.type = CellType::Wall,
                                            .constraints = {
                                                .north = {CellType::Grass},
                                                .east = {CellType::Wall},
                                                .south = {CellType::Wall},
                                                .west = {CellType::Wall},
                                            }});

    tileset_constraints.add_constraint(
        tiling::wang::TilesetIndex(3, 1),
        tiling::wang::TilesetTile<CellType>{.type = CellType::Wall,
                                            .constraints = {
                                                .north = {CellType::Grass},
                                                .east = {CellType::Grass},
                                                .south = {CellType::Wall},
                                                .west = {CellType::Wall},
                                            }});

    tileset_constraints.add_constraint(
        tiling::wang::TilesetIndex(1, 2),
        tiling::wang::TilesetTile<CellType>{.type = CellType::Wall,
                                            .constraints = {
                                                .north = {CellType::Wall},
                                                .east = {CellType::Wall},
                                                .south = {CellType::Wall},
                                                .west = {CellType::Grass},
                                            }});

    tileset_constraints.add_constraint(
        tiling::wang::TilesetIndex(3, 2),
        tiling::wang::TilesetTile<CellType>{.type = CellType::Wall,
                                            .constraints = {
                                                .north = {CellType::Wall},
                                                .east = {CellType::Grass},
                                                .south = {CellType::Wall},
                                                .west = {CellType::Wall},
                                            }});

    tileset_constraints.add_constraint(
        tiling::wang::TilesetIndex(0, 3),
        tiling::wang::TilesetTile<CellType>{
            .type = CellType::Grass,
            .constraints = {
                .north = {CellType::Grass, CellType::Wall, CellType::Grass,
                          CellType::Grass, CellType::Grass, CellType::Wall,
                          CellType::Wall, CellType::Wall, CellType::Grass,
                          CellType::Grass, CellType::Grass, CellType::Wall,
                          CellType::Wall, CellType::Wall, CellType::Grass,
                          CellType::Wall},
                .east = {CellType::Grass, CellType::Grass, CellType::Wall,
                         CellType::Grass, CellType::Grass, CellType::Wall,
                         CellType::Grass, CellType::Grass, CellType::Wall, CellType::Wall,
                         CellType::Grass, CellType::Wall, CellType::Wall, CellType::Grass,
                         CellType::Wall, CellType::Wall},
                .south = {CellType::Grass, CellType::Grass, CellType::Grass,
                          CellType::Wall, CellType::Grass, CellType::Grass,
                          CellType::Wall, CellType::Grass, CellType::Wall,
                          CellType::Grass, CellType::Wall, CellType::Wall,
                          CellType::Grass, CellType::Wall, CellType::Wall,
                          CellType::Wall},
                .west = {CellType::Grass, CellType::Grass, CellType::Grass,
                         CellType::Grass, CellType::Wall, CellType::Grass,
                         CellType::Grass, CellType::Wall, CellType::Grass, CellType::Wall,
                         CellType::Wall, CellType::Grass, CellType::Wall, CellType::Wall,
                         CellType::Wall, CellType::Wall},
            }});

    tileset_constraints.add_constraint(
        tiling::wang::TilesetIndex(1, 3),
        tiling::wang::TilesetTile<CellType>{
            .type = CellType::Wall,
            .constraints = {
                .north = {CellType::Wall, CellType::Grass},
                .east = {CellType::Wall, CellType::Wall},
                .south = {CellType::Grass, CellType::Grass},
                .west = {CellType::Grass, CellType::Grass},
            }});

    tileset_constraints.add_constraint(
        tiling::wang::TilesetIndex(2, 3),
        tiling::wang::TilesetTile<CellType>{
            .type = CellType::Wall,
            .constraints = {
                .north = {CellType::Wall, CellType::Wall},
                .east = {CellType::Wall, CellType::Grass},
                .south = {CellType::Grass, CellType::Grass},
                .west = {CellType::Wall, CellType::Grass},
            }});

    tileset_constraints.add_constraint(
        tiling::wang::TilesetIndex(3, 3),
        tiling::wang::TilesetTile<CellType>{
            .type = CellType::Wall,
            .constraints = {
                .north = {CellType::Wall, CellType::Grass},
                .east = {CellType::Grass, CellType::Grass},
                .south = {CellType::Grass, CellType::Grass},
                .west = {CellType::Wall, CellType::Wall},
            }});

    return tileset_constraints;
}
