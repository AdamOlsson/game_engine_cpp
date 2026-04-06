#pragma once

#include <cstdint>
#include <vector>

namespace tiled {

class Tileset {
  public:
    Tileset() = default;
    Tileset(int32_t tile_width, int32_t tile_height, int32_t tilecount, int32_t columns,
            std::vector<uint8_t> image_data, int32_t image_width, int32_t image_height);

    int32_t get_tile_width() const;
    int32_t get_tile_height() const;
    int32_t get_tilecount() const;
    int32_t get_columns() const;
    const uint8_t *get_image_data() const;
    int32_t get_image_width() const;
    int32_t get_image_height() const;

  private:
    int32_t m_tile_width = 0;
    int32_t m_tile_height = 0;
    int32_t m_tilecount = 0;
    int32_t m_columns = 0;
    std::vector<uint8_t> m_image_data;
    int32_t m_image_width = 0;
    int32_t m_image_height = 0;
};

} // namespace tiled
