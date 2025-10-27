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

    const auto wall_constraints = tiling::wang::TileConstraint<CellType>{
        .type = CellType::Wall,
        .north = {CellType::Wall, CellType::Grass},
        .east = {CellType::Wall, CellType::Grass},
        .south = {CellType::Wall, CellType::Grass},
        .west = {CellType::Wall, CellType::Grass},
    };
    constraints.add_constraint(tiling::wang::TilesetIndex(0, 0),
                               std::move(wall_constraints));

    auto rule = [](float value) -> CellType {
        if (value > 0.5) {
            return CellType::Wall;
        } else {
            return CellType::Grass;
        }
    };

    auto wang =
        tiling::wang::WangTiles<CellType>(noise_map, rule, std::move(constraints));

    auto tileset_index = wang.lookup_tile(1, 1);
    EXPECT_TRUE(tileset_index.has_value());
    ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index.value());

    tileset_index = wang.lookup_tile(2, 1);
    EXPECT_TRUE(tileset_index.has_value());
    ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index.value());

    // TODO: A problem is that the outer most tiles will always have a "None"
    // constraint towards the edge of the map. Either I would need to have a
    // this "None" to a wildcard match or even easier, the outer most tiles
    // always have no texture.
    tileset_index = wang.lookup_tile(0, 0);
    EXPECT_FALSE(tileset_index.has_value());
}
