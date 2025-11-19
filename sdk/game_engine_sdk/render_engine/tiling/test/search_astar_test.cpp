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

    EXPECT_EQ(path.size(), 0);
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

    EXPECT_EQ(path.size(), 1);
    EXPECT_EQ(path[0], end);
}
