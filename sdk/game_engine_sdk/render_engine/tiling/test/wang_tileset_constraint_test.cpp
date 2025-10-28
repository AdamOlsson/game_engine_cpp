#include <gtest/gtest.h>

#include "tiling/wang/TilesetConstraints.h"
#include "tiling/wang/TilesetIndex.h"

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

TEST(WangTilesetConstraintTest, Test_AddSingleConstraint) {
    auto constraints = tiling::wang::TilesetConstraints<CellType>();
    constraints.add_constraint(
        tiling::wang::TilesetIndex(0, 0),
        tiling::wang::TilesetTile<CellType>{.type = CellType::Wall,
                                            .constraints = {
                                                .north = {CellType::Wall},
                                                .east = {CellType::Wall},
                                                .south = {CellType::Wall},
                                                .west = {CellType::Wall},
                                            }});
    auto tileset_index = constraints.lookup_constraint(
        CellType::Wall, CellType::Wall, CellType::Wall, CellType::Wall, CellType::Wall);
    EXPECT_TRUE(tileset_index.has_value());
    ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index.value());
}

TEST(WangTilesetConstraintTest, Test_AllPermutationsGenerated) {
    {
        auto constraints = tiling::wang::TilesetConstraints<CellType>();
        constraints.add_constraint(tiling::wang::TilesetIndex(0, 0),
                                   tiling::wang::TilesetTile<CellType>{
                                       .type = CellType::Wall,
                                       .constraints = {
                                           .north = {CellType::Wall, CellType::Grass},
                                           .east = {CellType::Wall},
                                           .south = {CellType::Wall},
                                           .west = {CellType::Wall},
                                       }});

        ASSERT_EQ(2, constraints.count());

        auto tileset_index_1 =
            constraints.lookup_constraint(CellType::Wall, CellType::Wall, CellType::Wall,
                                          CellType::Wall, CellType::Wall);
        EXPECT_TRUE(tileset_index_1.has_value());
        ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index_1.value());

        auto tileset_index_2 =
            constraints.lookup_constraint(CellType::Wall, CellType::Grass, CellType::Wall,
                                          CellType::Wall, CellType::Wall);
        EXPECT_TRUE(tileset_index_2.has_value());
        ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index_2.value());
    }

    {
        auto constraints = tiling::wang::TilesetConstraints<CellType>();
        constraints.add_constraint(tiling::wang::TilesetIndex(0, 0),
                                   tiling::wang::TilesetTile<CellType>{
                                       .type = CellType::Wall,
                                       .constraints = {
                                           .north = {CellType::Wall},
                                           .east = {CellType::Wall, CellType::Grass},
                                           .south = {CellType::Wall},
                                           .west = {CellType::Wall},
                                       }});

        ASSERT_EQ(2, constraints.count());

        auto tileset_index_1 =
            constraints.lookup_constraint(CellType::Wall, CellType::Wall, CellType::Wall,
                                          CellType::Wall, CellType::Wall);
        EXPECT_TRUE(tileset_index_1.has_value());
        ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index_1.value());

        auto tileset_index_2 =
            constraints.lookup_constraint(CellType::Wall, CellType::Wall, CellType::Grass,
                                          CellType::Wall, CellType::Wall);
        EXPECT_TRUE(tileset_index_2.has_value());
        ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index_2.value());
    }

    {
        auto constraints = tiling::wang::TilesetConstraints<CellType>();
        constraints.add_constraint(tiling::wang::TilesetIndex(0, 0),
                                   tiling::wang::TilesetTile<CellType>{
                                       .type = CellType::Wall,
                                       .constraints = {
                                           .north = {CellType::Wall},
                                           .east = {CellType::Wall},
                                           .south = {CellType::Wall, CellType::Grass},
                                           .west = {CellType::Wall},
                                       }});

        ASSERT_EQ(2, constraints.count());

        auto tileset_index_1 =
            constraints.lookup_constraint(CellType::Wall, CellType::Wall, CellType::Wall,
                                          CellType::Wall, CellType::Wall);
        EXPECT_TRUE(tileset_index_1.has_value());
        ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index_1.value());

        auto tileset_index_2 =
            constraints.lookup_constraint(CellType::Wall, CellType::Wall, CellType::Wall,
                                          CellType::Grass, CellType::Wall);
        EXPECT_TRUE(tileset_index_2.has_value());
        ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index_2.value());
    }

    {
        auto constraints = tiling::wang::TilesetConstraints<CellType>();
        constraints.add_constraint(tiling::wang::TilesetIndex(0, 0),
                                   tiling::wang::TilesetTile<CellType>{
                                       .type = CellType::Wall,
                                       .constraints = {
                                           .north = {CellType::Wall},
                                           .east = {CellType::Wall},
                                           .south = {CellType::Wall},
                                           .west = {CellType::Wall, CellType::Grass},
                                       }});

        ASSERT_EQ(2, constraints.count());

        auto tileset_index_1 =
            constraints.lookup_constraint(CellType::Wall, CellType::Wall, CellType::Wall,
                                          CellType::Wall, CellType::Wall);
        EXPECT_TRUE(tileset_index_1.has_value());
        ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index_1.value());

        auto tileset_index_2 =
            constraints.lookup_constraint(CellType::Wall, CellType::Wall, CellType::Wall,
                                          CellType::Wall, CellType::Grass);
        EXPECT_TRUE(tileset_index_2.has_value());
        ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index_2.value());
    }

    {
        auto constraints = tiling::wang::TilesetConstraints<CellType>();
        constraints.add_constraint(tiling::wang::TilesetIndex(0, 0),
                                   tiling::wang::TilesetTile<CellType>{
                                       .type = CellType::Wall,
                                       .constraints = {
                                           .north = {CellType::Wall, CellType::Grass},
                                           .east = {CellType::Wall},
                                           .south = {CellType::Wall},
                                           .west = {CellType::Wall, CellType::Water},
                                       }});

        ASSERT_EQ(4, constraints.count());

        auto tileset_index_1 =
            constraints.lookup_constraint(CellType::Wall, CellType::Wall, CellType::Wall,
                                          CellType::Wall, CellType::Wall);
        EXPECT_TRUE(tileset_index_1.has_value());
        ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index_1.value());

        auto tileset_index_2 =
            constraints.lookup_constraint(CellType::Wall, CellType::Grass, CellType::Wall,
                                          CellType::Wall, CellType::Wall);
        EXPECT_TRUE(tileset_index_2.has_value());
        ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index_2.value());

        auto tileset_index_3 =
            constraints.lookup_constraint(CellType::Wall, CellType::Wall, CellType::Wall,
                                          CellType::Wall, CellType::Water);
        EXPECT_TRUE(tileset_index_3.has_value());
        ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index_3.value());

        auto tileset_index_4 =
            constraints.lookup_constraint(CellType::Wall, CellType::Grass, CellType::Wall,
                                          CellType::Wall, CellType::Water);
        EXPECT_TRUE(tileset_index_4.has_value());
        ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index_4.value());
    }
}

TEST(WangTilesetConstraintTest,
     Test_TwoTileWithDifferentClassificationButSameConstraints) {
    auto constraints = tiling::wang::TilesetConstraints<CellType>();

    auto wall_constraint = tiling::wang::TilesetTile<CellType>{
        .type = CellType::Wall,
        .constraints =
            {
                .north = {CellType::Wall},
                .east = {CellType::Wall},
                .south = {CellType::Wall},
                .west = {CellType::Wall},
            },
    };
    constraints.add_constraint(tiling::wang::TilesetIndex(0, 0),
                               std::move(wall_constraint));

    auto grass_constraint = tiling::wang::TilesetTile<CellType>{
        .type = CellType::Grass,
        .constraints =
            {
                .north = {CellType::Wall},
                .east = {CellType::Wall},
                .south = {CellType::Wall},
                .west = {CellType::Wall},
            },
    };
    constraints.add_constraint(tiling::wang::TilesetIndex(1, 1),
                               std::move(grass_constraint));

    auto tileset_index_wall = constraints.lookup_constraint(
        CellType::Wall, CellType::Wall, CellType::Wall, CellType::Wall, CellType::Wall);
    EXPECT_TRUE(tileset_index_wall.has_value());
    ASSERT_EQ(tiling::wang::TilesetIndex(0, 0), tileset_index_wall.value());

    auto tileset_index_grass = constraints.lookup_constraint(
        CellType::Grass, CellType::Wall, CellType::Wall, CellType::Wall, CellType::Wall);
    EXPECT_TRUE(tileset_index_grass.has_value());
    ASSERT_EQ(tiling::wang::TilesetIndex(1, 1), tileset_index_grass.value());
}
