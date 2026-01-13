#pragma once

#include "font/OTFFont.h"
#include <iostream>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace font {

class OutlineBuilder {
  public:
    OutlineBuilder()
        : m_funcs({
              .move_to = &move_to_cb,
              .line_to = &line_to_cb,
              .conic_to = &conic_to_cb,
              .cubic_to = &cubic_to_cb,
              .shift = 0,
              .delta = 0,
          }) {}

    ~OutlineBuilder() {}

    FT_Outline_Funcs *funcs() { return &m_funcs; }

  private:
    FT_Outline_Funcs m_funcs;

    static OutlineBuilder &self(void *user_data) {
        return *static_cast<OutlineBuilder *>(user_data);
    }

    static int move_to_cb(const FT_Vector *to, void *user_data) {
        std::cout << std::format("M {} {}", to->x, to->y) << std::endl;
        return 0;
    }

    static int line_to_cb(const FT_Vector *to, void *user_data) {
        std::cout << std::format("L {} {}", to->x, to->y) << std::endl;
        return 0;
    }

    static int conic_to_cb(const FT_Vector *control, const FT_Vector *to,
                           void *user_data) {
        std::cout << std::format("C {} {} {} {}", control->x, control->y, to->x, to->y)
                  << std::endl;
        return 0;
    }

    static int cubic_to_cb(const FT_Vector *control1, const FT_Vector *control2,
                           const FT_Vector *to, void *user_data) {
        std::cout << std::format("C {} {} {} {} {} {}", to->x, to->y, control1->x,
                                 control1->y, control2->x, control2->y)
                  << std::endl;
        return 0;
    }
};

class FontLoader {
  public:
    FontLoader(const std::string &filepath);
    FontLoader(const std::vector<char> &font_data);

    ~FontLoader();

    unsigned int get_glyph_index(const font::Unicode &codepoint);
    void get_glyph_outline(const font::Unicode &codepoint);

  private:
    OutlineBuilder m_builder;
    FT_Library m_library;
    FT_Face m_face;
};

} // namespace font
