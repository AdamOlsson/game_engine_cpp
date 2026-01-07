#pragma once

#include "font/Glyph.h"
#include "font/Unicode.h"
#include "font/detail/otf_font/FontTableCmap.h"
#include "font/detail/otf_font/SfntHeader.h"
#include <string>

namespace font {

class OTFFont {
  private:
    font::detail::otf_font::cmap::FontTableCmap m_font_table_cmap;

    uint32_t calculate_table_checksum(const std::vector<uint8_t> &table);
    bool validate_table(std::ifstream &stream,
                        const font::detail::otf_font::TableRecord &record);

  public:
    std::vector<Glyph> glyphs;
    OTFFont() = default;

    OTFFont(const std::string &filepath);

    size_t glyph_index(const Unicode &unicode) const;
};

}; // namespace font
