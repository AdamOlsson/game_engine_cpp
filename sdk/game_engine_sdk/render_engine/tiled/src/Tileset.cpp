#include "Tileset.h"

namespace tiled {

Tileset::Tileset(int32_t tile_width, int32_t tile_height, int32_t tilecount,
                 int32_t columns, std::vector<uint8_t> image_data, int32_t image_width,
                 int32_t image_height)
    : m_tile_width(tile_width), m_tile_height(tile_height), m_tilecount(tilecount),
      m_columns(columns), m_image_data(std::move(image_data)), m_image_width(image_width),
      m_image_height(image_height) {}

int32_t Tileset::get_tile_width() const { return m_tile_width; }

int32_t Tileset::get_tile_height() const { return m_tile_height; }

int32_t Tileset::get_tilecount() const { return m_tilecount; }

int32_t Tileset::get_columns() const { return m_columns; }

const uint8_t *Tileset::get_image_data() const { return m_image_data.data(); }

int32_t Tileset::get_image_width() const { return m_image_width; }

int32_t Tileset::get_image_height() const { return m_image_height; }

} // namespace tiled
