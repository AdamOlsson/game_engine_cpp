#pragma once
#include <vector>
namespace font {

using Outline = std::vector<std::pair<int, int>>;
using GlyphOutlines = std::vector<Outline>;

struct Glyph {
    std::string name;
    GlyphOutlines outlines;
};

} // namespace font
