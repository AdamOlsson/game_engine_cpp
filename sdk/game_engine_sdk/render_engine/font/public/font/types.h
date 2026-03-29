#pragma once

#include "math/Bbox.h"
#include "math/Vector3.h"
#include "util/colors.h"

namespace font {

struct Glyph {
    char32_t unicode_char = 0;
    size_t first_index = 0;
    size_t index_count = 0;
    math::Vector2 offset;
};

struct Text {
    math::Bbox bbox;
    std::vector<Glyph> glyphs;
};

struct TextFormat {
    math::Vector3 position = math::Vector3(0.0f);
    float font_size = 11.0f;
    util::colors::Color font_color = util::colors::WHITE;
};

} // namespace font
