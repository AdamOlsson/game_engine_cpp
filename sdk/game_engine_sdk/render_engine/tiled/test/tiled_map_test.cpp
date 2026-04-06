#include "tiled/TiledMap.h"
#include <gtest/gtest.h>

namespace tiled {
namespace test {

class TiledMapTest : public ::testing::Test {
  protected:
    std::filesystem::path get_test_path() const {
        return std::filesystem::path(
            "/Users/adamolsson/projects/game_engine_cpp/sdk/game_engine_sdk/"
            "render_engine/tiled/test/assets/test_map.tmx");
    }
};

TEST_F(TiledMapTest, ParsesTilesetBasename) {
    TiledMap map(get_test_path());
    EXPECT_EQ(map.get_tileset_basename(), "TopDownFantasy-Forest.tsx");
}

TEST_F(TiledMapTest, ParsesRenderOrder) {
    TiledMap map(get_test_path());
    EXPECT_EQ(map.get_render_order(), RenderOrder::RightDown);
}

TEST_F(TiledMapTest, ParsesTileDimensions) {
    TiledMap map(get_test_path());
    EXPECT_EQ(map.get_tile_width(), 16);
    EXPECT_EQ(map.get_tile_height(), 16);
}

TEST_F(TiledMapTest, ParsesChunkDimensions) {
    TiledMap map(get_test_path());
    EXPECT_EQ(map.get_chunk_width(), 16);
    EXPECT_EQ(map.get_chunk_height(), 16);
}

TEST_F(TiledMapTest, ReturnsCorrectLayerCount) {
    TiledMap map(get_test_path());
    EXPECT_EQ(map.get_layer_count(), 1);
}

TEST_F(TiledMapTest, ReturnsCorrectChunkCount) {
    TiledMap map(get_test_path());
    EXPECT_EQ(map.get_chunk_count(0), 2);
}

TEST_F(TiledMapTest, ChunkHasCorrectDimensions) {
    TiledMap map(get_test_path());
    const auto &chunk = map.get_chunk(0, 0);
    EXPECT_EQ(chunk.width, 16);
    EXPECT_EQ(chunk.height, 16);
}

TEST_F(TiledMapTest, ChunkHasCorrectPosition) {
    TiledMap map(get_test_path());
    const auto &chunk = map.get_chunk(0, 0);
    EXPECT_EQ(chunk.x, -16);
    EXPECT_EQ(chunk.y, -16);
}

TEST_F(TiledMapTest, ChunkHasCorrectTileCount) {
    TiledMap map(get_test_path());
    const auto &chunk = map.get_chunk(0, 0);
    EXPECT_GE(chunk.tiles.size(), 256u);
}

TEST_F(TiledMapTest, CanIndexTile) {
    TiledMap map(get_test_path());
    uint32_t tile = map.get_tile(0, 0, 0);
    EXPECT_EQ(tile, 0);
}

TEST_F(TiledMapTest, CanAccessSecondChunk) {
    TiledMap map(get_test_path());
    const auto &chunk = map.get_chunk(0, 1);
    EXPECT_EQ(chunk.x, 0);
    EXPECT_EQ(chunk.y, -16);
}

} // namespace test
} // namespace tiled
