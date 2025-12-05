
#pragma once

#include "font/detail/otf_font/FontTableHead.h"
#include "font/detail/otf_font/FontTableHhea.h"
#include "font/detail/otf_font/SfntHeader.h"
#include "font/detail/otf_font/cff/FontTableCFF.h"
#include <string>
namespace font {

class OTFFont {
  private:
    font::detail::otf_font::SFntHeader m_sfnt_header;
    font::detail::otf_font::FontTableHead m_font_table_head;
    font::detail::otf_font::FontTableHhea m_font_table_hhea;

  public:
    font::detail::otf_font::cff::FontTableCFF m_font_table_cff;
    OTFFont(const std::string &filepath);
};

}; // namespace font
