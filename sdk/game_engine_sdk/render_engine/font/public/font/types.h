#pragma once

#include "math/Bbox.h"
#include "math/Matrix.h"

namespace font {

struct Glyph {
    char32_t unicode_char = 0;
    size_t first_index = 0;
    size_t index_count = 0;
    math::Matrix model_matrix = glm::mat4(1.0f);
};

struct Text {
    math::Bbox bbox;
    std::vector<Glyph> glyphs;
};

} // namespace font
