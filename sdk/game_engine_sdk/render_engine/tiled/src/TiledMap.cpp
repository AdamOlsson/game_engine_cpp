#include "tiled/TiledMap.h"
#include "TiledMapLoader.h"
#include "TilesetLoader.h"

namespace tiled {

TiledMap::TiledMap(const std::filesystem::path &path) {
    TiledMapLoader loader(path);

    m_tileset_basename = loader.get_tileset_basename();
    m_render_order = loader.get_render_order();
    m_tile_width = loader.get_tile_width();
    m_tile_height = loader.get_tile_height();

    m_layers.resize(loader.get_layer_count());
    for (size_t i = 0; i < loader.get_layer_count(); ++i) {
        m_layers[i] = loader.get_layer(i);
    }

    if (!m_layers.empty() && !m_layers[0].empty()) {
        m_chunk_width = m_layers[0][0].width;
        m_chunk_height = m_layers[0][0].height;
    }

    std::filesystem::path tileset_path = loader.get_tileset_path();
    if (!tileset_path.empty()) {
        std::filesystem::path tileset_dir = tileset_path.parent_path();
        Tileset tileset = TilesetLoader::load_from_file(tileset_path, tileset_dir);

        m_tileset_tile_width = tileset.get_tile_width();
        m_tileset_tile_height = tileset.get_tile_height();
        m_tileset_tilecount = tileset.get_tilecount();
        m_tileset_columns = tileset.get_columns();
        m_tileset_image_width = tileset.get_image_width();
        m_tileset_image_height = tileset.get_image_height();

        const uint8_t *img_data = tileset.get_image_data();
        if (img_data) {
            size_t img_size = static_cast<size_t>(m_tileset_image_width) *
                              static_cast<size_t>(m_tileset_image_height) * 4;
            m_tileset_image_data.assign(img_data, img_data + img_size);
        }
    }
}

std::string TiledMap::get_tileset_basename() const { return m_tileset_basename; }

RenderOrder TiledMap::get_render_order() const { return m_render_order; }

int32_t TiledMap::get_tile_width() const { return m_tile_width; }

int32_t TiledMap::get_tile_height() const { return m_tile_height; }

int32_t TiledMap::get_chunk_width() const { return m_chunk_width; }

int32_t TiledMap::get_chunk_height() const { return m_chunk_height; }

const uint8_t *TiledMap::get_tileset_image_data() const {
    return m_tileset_image_data.empty() ? nullptr : m_tileset_image_data.data();
}

int32_t TiledMap::get_tileset_image_width() const { return m_tileset_image_width; }

int32_t TiledMap::get_tileset_image_height() const { return m_tileset_image_height; }

int32_t TiledMap::get_tileset_tile_width() const { return m_tileset_tile_width; }

int32_t TiledMap::get_tileset_tile_height() const { return m_tileset_tile_height; }

int32_t TiledMap::get_tileset_tilecount() const { return m_tileset_tilecount; }

int32_t TiledMap::get_tileset_columns() const { return m_tileset_columns; }

math::Vector4 TiledMap::get_tile_uvwt(uint32_t tile_id) const {
    if (tile_id == 0 || m_tileset_columns == 0 || m_tileset_tile_width == 0 ||
        m_tileset_tile_height == 0 || m_tileset_image_width == 0 ||
        m_tileset_image_height == 0) {
        return math::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    uint32_t index = tile_id - 1;
    uint32_t col = index % static_cast<uint32_t>(m_tileset_columns);
    uint32_t row = index / static_cast<uint32_t>(m_tileset_columns);

    float u = static_cast<float>(col * m_tileset_tile_width) /
              static_cast<float>(m_tileset_image_width);
    float v = static_cast<float>(row * m_tileset_tile_height) /
              static_cast<float>(m_tileset_image_height);
    float w = static_cast<float>((col + 1) * m_tileset_tile_width) /
              static_cast<float>(m_tileset_image_width);
    float t = static_cast<float>((row + 1) * m_tileset_tile_height) /
              static_cast<float>(m_tileset_image_height);

    return math::Vector4(u, v, w, t);
}

size_t TiledMap::get_layer_count() const { return m_layers.size(); }

size_t TiledMap::get_chunk_count() const {
    size_t count = 0;
    for (auto &l : m_layers) {
        count += l.size();
    }
    return count;
}

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

math::Vector2 Chunk::get_tile_position(size_t index) const {
    if (index >= tiles.size() || width == 0) {
        return math::Vector2(0.0f, 0.0f);
    }

    int32_t col = static_cast<int32_t>(index % static_cast<size_t>(width));
    int32_t row = static_cast<int32_t>(index / static_cast<size_t>(width));

    int32_t tile_x = x + col;
    int32_t tile_y = y + row;

    return math::Vector2(static_cast<float>(tile_x), static_cast<float>(tile_y));
}

} // namespace tiled
