#include "game_engine_sdk/render_engine/TilesetUVWT.h"

TilesetUVWT::TilesetUVWT(Texture &tileset, TileSize size) {
    const auto dimension = tileset.dimension();
    m_norm_tile_size = glm::vec2(size.x / dimension.width, size.y / dimension.height);
}

glm::vec4 TilesetUVWT::uvwt_for_tile_at(unsigned int x, unsigned y) {
    return glm::vec4(m_norm_tile_size.x * static_cast<float>(x),
                     m_norm_tile_size.y * static_cast<float>(y),
                     m_norm_tile_size.x * static_cast<float>(x + 1),
                     m_norm_tile_size.y * static_cast<float>(y + 1));
}
