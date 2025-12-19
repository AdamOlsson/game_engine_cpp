#pragma once

#include "font/Glyph.h"
#include "font/Unicode.h"
#include "font/detail/otf_font/FontTableCmap.h"
#include <string>

namespace font {

class OTFFont {
  private:
    font::detail::otf_font::cmap::FontTableCmap m_font_table_cmap;

  public:
    std::vector<Glyph> glyphs;

    OTFFont(const std::string &filepath);

    size_t glyph_index(const Unicode &unicode);
};

}; // namespace font
