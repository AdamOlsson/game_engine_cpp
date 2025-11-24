#include "logger/logger.h"
#include "tiling/TileGrid.h"
#include "tiling/search/AStar.h"
#include <gtest/gtest.h>

enum class CellType : uint8_t {};

TEST(SearchAStarTest, Test_StartAndEndAreSame_ExpectEmptyList) {
    const auto width = 100;
    const auto height = 100;
    auto tile_grid = tiling::TileGrid<CellType>(width, height);

    for (auto &tile : tile_grid) {
        tile.weight = 1.0f;
    }

    const auto start = tiling::Position(50, 50);
    const auto end = tiling::Position(50, 50);

    const auto path = tiling::search::AStar::search(tile_grid, start, end);

    EXPECT_EQ(path.size(), 2);
    EXPECT_EQ(path[0], start);
    EXPECT_EQ(path[1], end);
}

TEST(SearchAStarTest, Test_StartAndEndAreNextToEachOther_ExpectOneStep) {
    const auto width = 100;
    const auto height = 100;
    auto tile_grid = tiling::TileGrid<CellType>(width, height);

    for (auto &tile : tile_grid) {
        tile.weight = 1.0f;
    }

    const auto start = tiling::Position(50, 50);
    const auto end = tiling::Position(51, 50);

    const auto path = tiling::search::AStar::search(tile_grid, start, end);

    EXPECT_EQ(path.size(), 2);
    EXPECT_EQ(path[0], start);
    EXPECT_EQ(path[1], end);
}

TEST(SearchAStarTest, Test_BestPathFound) {
    std::vector<std::vector<tiling::Tile<CellType>>> weights = {
        {{.weight = 1.0f}, {.weight = 1.0f}, {.weight = 1.0f}},
        {{.weight = 1.0f}, {.weight = 10.0f}, {.weight = 1.0f}},
        {{.weight = 1.0f}, {.weight = 10.0f}, {.weight = 1.0f}},
        {{.weight = 1.0f}, {.weight = 1.0f}, {.weight = 1.0f}},
    };

    auto tile_grid = tiling::TileGrid<CellType>(weights);

    // Note: col and row are reversed compared to x and y
    const auto start_row = 3;
    const auto start_col = 0;
    const auto start = tiling::Position(start_col, start_row);
    const auto end_row = 2;
    const auto end_col = 2;
    const auto end = tiling::Position(end_col, end_row);

    const auto path = tiling::search::AStar::search(tile_grid, start, end);

    EXPECT_EQ(path.size(), 3);
    EXPECT_EQ(path[0], start);
    EXPECT_EQ(path[1], tiling::Position(1, 3));
    EXPECT_EQ(path[2], end);
}
