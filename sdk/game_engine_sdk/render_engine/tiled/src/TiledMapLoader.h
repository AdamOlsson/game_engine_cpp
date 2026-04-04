#pragma once

#include "tiled/TiledMap.h"

namespace tiled {

class TiledMapLoader {
  public:
    explicit TiledMapLoader(const std::filesystem::path &path);

    std::string get_tileset_basename() const;
    RenderOrder get_render_order() const;

    size_t get_layer_count() const;
    const Layer &get_layer(size_t index) const;
    const Chunk &get_chunk(size_t layer_index, size_t chunk_index) const;
    uint32_t get_tile(size_t layer_index, size_t chunk_index, size_t tile_index) const;

  private:
    std::string m_tileset_basename;
    RenderOrder m_render_order = RenderOrder::RightDown;
    std::vector<Layer> m_layers;
    std::vector<std::string> m_layer_names;

    void parse_file(const std::filesystem::path &path);
    void parse_map_element(const std::string &content);
    void parse_layer_element(const std::string &content);
    std::string extract_attribute(const std::string &element,
                                  const std::string &attribute);
    Layer parse_chunks(const std::string &data_content);
    Chunk parse_single_chunk(const std::string &chunk_element);
    std::vector<uint32_t> parse_csv(const std::string &csv_content);
};

} // namespace tiled
