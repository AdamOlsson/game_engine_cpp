#include "tiling/NoiseMap.h"
#include "tiling/wang/TilesetConstraints.h"
#include "tiling/wang/WangTiles.h"
#include <gtest/gtest.h>

enum class CellType : uint8_t {
    None,
    Wall,
    Grass,
    Water,
};

inline std::ostream &operator<<(std::ostream &os, CellType type) {
    switch (type) {
    case CellType::None:
        return os << "CellType::None";
    case CellType::Wall:
        return os << "CellType::Wall";
    case CellType::Grass:
        return os << "CellType::Grass";
    case CellType::Water:
        return os << "CellType::Water";
    }
}

TEST(WangTilesetConstraintTest, Test_BasicTileSetup) {
    const float empty = 0.0f;
    const float wall = 1.0f;

    const unsigned int noise_width = 4;
    const unsigned int noise_height = 3;
    // clang-format off
    std::vector<float> noise = {
        empty, empty, empty, empty,
        empty, wall , wall , empty,
        empty, empty, empty, empty,
    };
    // clang-format on
    auto noise_map = tiling::NoiseMap(std::move(noise), noise_width, noise_height);

    auto constraints = tiling::wang::TilesetConstraints<CellType>();

    const auto wall_constraints = tiling::wang::TilesetTile<CellType>{
        .type = CellType::Wall,
        .constraints = {
            .north = {CellType::Grass, CellType::Grass},
            .east = {CellType::Wall, CellType::Grass},
            .south = {CellType::Grass, CellType::Grass},
            .west = {CellType::Grass, CellType::Wall},
        }};
    constraints.add_constraint(tiling::TilesetIndex(0, 0), std::move(wall_constraints));

    auto rule = [](float value) -> tiling::Tile<CellType> {
        if (value > 0.5) {
            return tiling::Tile{.type = CellType::Wall};
        } else {
            return tiling::Tile{.type = CellType::Grass};
        }
    };

    auto grid = tiling::TileGrid<CellType>(noise_map.width, noise_map.height);

    for (auto i = 0; i < noise_map.width * noise_map.height; i++) {
        grid[i] = rule(noise_map.noise[i]);
    }
    tiling::wang::assign_tile_sprites(grid, constraints);

    auto tileset_index = grid.get_cell(1, 1).sprite_index;
    EXPECT_TRUE(tileset_index.has_value());
    ASSERT_EQ(tiling::TilesetIndex(0, 0), tileset_index.value());

    tileset_index = grid.get_cell(2, 1).sprite_index;
    EXPECT_TRUE(tileset_index.has_value());
    ASSERT_EQ(tiling::TilesetIndex(0, 0), tileset_index.value());

    // TODO: A problem is that the outer most tiles will always have a "None"
    // constraint towards the edge of the map. Either I would need to have a
    // this "None" to a wildcard match or even easier, the outer most tiles
    // always have no texture.
    tileset_index = grid.get_cell(0, 0).sprite_index;
    EXPECT_FALSE(tileset_index.has_value());
}
