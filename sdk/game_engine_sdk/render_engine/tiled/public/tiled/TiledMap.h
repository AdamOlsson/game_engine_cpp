#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "Tileset.h"
#include "math/Vector2.h"
#include "math/Vector4.h"

namespace tiled {

enum class RenderOrder { RightDown, RightUp, LeftDown, LeftUp };

struct Chunk {
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;
    std::vector<uint32_t> tiles;

    math::Vector2 get_tile_position(size_t index) const;
};

using Layer = std::vector<Chunk>;

class TiledMap {
  public:
    explicit TiledMap(const std::filesystem::path &path);

    std::string get_tileset_basename() const;
    RenderOrder get_render_order() const;
    int32_t get_tile_width() const;
    int32_t get_tile_height() const;
    int32_t get_chunk_width() const;
    int32_t get_chunk_height() const;

    const uint8_t *get_tileset_image_data() const;
    int32_t get_tileset_image_width() const;
    int32_t get_tileset_image_height() const;
    int32_t get_tileset_tile_width() const;
    int32_t get_tileset_tile_height() const;
    int32_t get_tileset_tilecount() const;
    int32_t get_tileset_columns() const;

    math::Vector4 get_tile_uvwt(uint32_t tile_id) const;

    size_t get_layer_count() const;
    size_t get_chunk_count() const;
    size_t get_chunk_count(size_t layer_index) const;
    const Layer &get_layer(size_t index) const;
    const Chunk &get_chunk(size_t layer_index, size_t chunk_index) const;
    uint32_t get_tile(size_t layer_index, size_t chunk_index, size_t tile_index) const;

  private:
    std::string m_tileset_basename;
    RenderOrder m_render_order = RenderOrder::RightDown;
    int32_t m_tile_width = 0;
    int32_t m_tile_height = 0;
    int32_t m_chunk_width = 0;
    int32_t m_chunk_height = 0;
    std::vector<Layer> m_layers;

    std::optional<Tileset> m_tileset = std::nullopt;
};

} // namespace tiled
