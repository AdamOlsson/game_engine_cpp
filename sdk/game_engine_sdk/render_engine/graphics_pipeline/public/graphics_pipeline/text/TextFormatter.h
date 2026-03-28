#pragma once

#include "font/FontLoader.h"
#include "graphics_pipeline/text/TextOpts.h"
#include "math/Bbox.h"
#include "math/Vector2.h"
#include "math/Vector4.h"
#include "util/assert.h"

#include <cstddef>

namespace graphics_pipeline::text {

struct Word {
    bool is_control_char = false;
    const size_t start_idx = 0;
    const size_t end_idx = std::numeric_limits<size_t>::max();
    math::Vector2 offset;
    math::Vector2 advance;
    math::Vector4 bbox;
    std::vector<math::Vector2> glyph_positions;
};

struct Text {
    size_t char_count = 0;
    math::Bbox bbox;
    std::vector<Word> words;
};

class TextFormatter {
  private:
    static constexpr char SPACE = U' ';
    static constexpr char LF = U'\n';

    font::FontLoader *m_font_loader = nullptr;

    Text layout_text(const font::Unicode &codepoint, const TextOpts &opts);
    Word layout_word(const font::Unicode &codepoint, const size_t start, const size_t end,
                     const math::Vector2 &offset);

  public:
    TextFormatter() = default;

    TextFormatter(font::FontLoader *font_loader) : m_font_loader(font_loader) {};

    TextFormatter(const TextFormatter &) = delete;
    TextFormatter(TextFormatter &&) noexcept = default;
    TextFormatter &operator=(const TextFormatter &) = delete;
    TextFormatter &operator=(TextFormatter &&) noexcept = default;

    Text format(const font::Unicode &codepoint, const TextOpts &opts) {
        DEBUG_ASSERT(m_font_loader != nullptr,
                     "Error: Trying to format code when the font loader is not set.");
        return layout_text(codepoint, opts);
    }
};

} // namespace graphics_pipeline::text
