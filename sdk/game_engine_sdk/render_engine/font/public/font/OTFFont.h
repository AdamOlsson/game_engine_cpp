
#pragma once

#include "font/Unicode.h"
#include "font/detail/otf_font/FontTableCmap.h"
#include "font/detail/otf_font/FontTableHead.h"
#include "font/detail/otf_font/FontTableHhea.h"
#include "font/detail/otf_font/SfntHeader.h"
#include "font/detail/otf_font/cff/FontTableCFF.h"
#include <string>
namespace font {
/*using char32_t * = Unicode;*/

class OTFFont {
  private:
    font::detail::otf_font::SFntHeader m_sfnt_header;
    font::detail::otf_font::FontTableHead m_font_table_head;
    font::detail::otf_font::FontTableHhea m_font_table_hhea;
    font::detail::otf_font::cmap::FontTableCmap m_font_table_cmap;

  public:
    font::detail::otf_font::cff::FontTableCFF m_font_table_cff;
    OTFFont(const std::string &filepath);

    size_t glyph_index(const Unicode &unicode);
};

}; // namespace font
