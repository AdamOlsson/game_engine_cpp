#pragma once

#include "math/Vector2.h"
#include "util/colors.h"

namespace font {

struct TextOpts {
    math::Vector2 position = math::Vector2(0, 0);
    util::colors::Color font_color = util::colors::WHITE;
    float font_size = 11;
    float line_width = 100.0f;
    float line_height = 0.0f;
};

} // namespace font
