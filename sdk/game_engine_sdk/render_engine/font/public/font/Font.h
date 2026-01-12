#pragma once

#include "ttfparser.h"
#include <cstdint>
#include <string>
#include <vector>

namespace font {

class FontLoader {
  public:
    FontLoader();
    ~FontLoader();

    bool load_font(const std::string &path);

    // Exposing ttf-parser types directly
    ttfp_face *get_face() const { return m_face; }

  private:
    ttfp_face *m_face;
    std::vector<uint8_t> m_fontData;
};

} // namespace font
