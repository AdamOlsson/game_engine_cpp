#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace tiled {

enum class RenderOrder { RightDown, RightUp, LeftDown, LeftUp };

struct Chunk {
    int32_t x = 0;
    int32_t y = 0;
    int32_t width = 0;
    int32_t height = 0;
    std::vector<uint32_t> tiles;
};

using Layer = std::vector<Chunk>;

class TiledMap {
  public:
    explicit TiledMap(const std::filesystem::path &path);

    std::string get_tileset_basename() const;
    RenderOrder get_render_order() const;

    size_t get_layer_count() const;
    size_t get_chunk_count(size_t layer_index) const;
    const Layer &get_layer(size_t index) const;
    const Chunk &get_chunk(size_t layer_index, size_t chunk_index) const;
    uint32_t get_tile(size_t layer_index, size_t chunk_index, size_t tile_index) const;

  private:
    std::string m_tileset_basename;
    RenderOrder m_render_order = RenderOrder::RightDown;
    std::vector<Layer> m_layers;
};

} // namespace tiled
