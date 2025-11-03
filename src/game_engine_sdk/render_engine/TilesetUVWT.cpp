#include "game_engine_sdk/render_engine/TilesetUVWT.h"

TilesetUVWT::TilesetUVWT(Texture &tileset, TileSize size) : m_tile_size(size) {
    const auto dimension = tileset.dimension();
    m_atlas_width = dimension.width;
    m_atlas_height = dimension.height;
}

glm::vec4 TilesetUVWT::uvwt_for_tile_at(unsigned int x, unsigned int y) {
    return glm::vec4((static_cast<float>(x) * m_tile_size.x + 0.5f) / m_atlas_width,
                     (static_cast<float>(y) * m_tile_size.y + 0.5f) / m_atlas_height,
                     (static_cast<float>(x + 1) * m_tile_size.x - 0.5f) / m_atlas_width,
                     (static_cast<float>(y + 1) * m_tile_size.y - 0.5f) / m_atlas_height);
}
