#include "tiled/Tileset.h"
#include "util/file.h"

namespace tiled {

Tileset::Tileset(int32_t tile_width, int32_t tile_height, int32_t tilecount,
                 int32_t columns, const std::filesystem::path &tileset_path,
                 int32_t image_width, int32_t image_height)
    : m_tile_width(tile_width), m_tile_height(tile_height), m_tilecount(tilecount),
      m_columns(columns), m_image_width(image_width), m_image_height(image_height) {

    const auto bytes = util::file::read_file(tileset_path.string());
    m_image_data = image::Image::load_rgba_image(
        reinterpret_cast<const uint8_t *>(bytes.data()), bytes.size());
}

int32_t Tileset::get_tile_width() const { return m_tile_width; }

int32_t Tileset::get_tile_height() const { return m_tile_height; }

int32_t Tileset::get_tilecount() const { return m_tilecount; }

int32_t Tileset::get_columns() const { return m_columns; }

const unsigned char *Tileset::get_image_data() const { return m_image_data.pixels; }

int32_t Tileset::get_image_width() const { return m_image_width; }

int32_t Tileset::get_image_height() const { return m_image_height; }

} // namespace tiled
