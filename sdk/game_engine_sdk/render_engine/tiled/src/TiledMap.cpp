#include "tiled/TiledMap.h"
#include "TiledMapLoader.h"

namespace tiled {

TiledMap::TiledMap(const std::filesystem::path &path) {
    TiledMapLoader loader(path);

    m_tileset_basename = loader.get_tileset_basename();
    m_render_order = loader.get_render_order();

    m_layers.resize(loader.get_layer_count());
    for (size_t i = 0; i < loader.get_layer_count(); ++i) {
        m_layers[i] = loader.get_layer(i);
    }
}

std::string TiledMap::get_tileset_basename() const { return m_tileset_basename; }

RenderOrder TiledMap::get_render_order() const { return m_render_order; }

size_t TiledMap::get_layer_count() const { return m_layers.size(); }

size_t TiledMap::get_chunk_count(size_t layer_index) const {
    return m_layers[layer_index].size();
}

const Layer &TiledMap::get_layer(size_t index) const { return m_layers[index]; }

const Chunk &TiledMap::get_chunk(size_t layer_index, size_t chunk_index) const {
    return m_layers[layer_index][chunk_index];
}

uint32_t TiledMap::get_tile(size_t layer_index, size_t chunk_index,
                            size_t tile_index) const {
    return m_layers[layer_index][chunk_index].tiles[tile_index];
}

} // namespace tiled
