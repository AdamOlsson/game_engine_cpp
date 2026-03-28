#pragma once

#include "font/FontLoader.h"
#include "font/TextOpts.h"
#include "math/Bbox.h"
#include "math/Vector2.h"
#include "math/Vector4.h"

#include <cstddef>

namespace font {

struct Word {
    bool is_control_char = false;
    const size_t start_idx = 0;
    const size_t end_idx = std::numeric_limits<size_t>::max();
    math::Vector2 offset;
    math::Vector2 advance;
    math::Vector4 bbox;
    std::vector<math::Vector2> glyph_positions;
};

struct TextLayout {
    size_t char_count = 0;
    math::Bbox bbox;
    std::vector<Word> words;
};

class TextFormatter {
  private:
    static constexpr char SPACE = U' ';
    static constexpr char LF = U'\n';

    font::FontLoader *m_font_loader = nullptr;

    TextLayout layout_text(const font::Unicode &codepoint, const TextOpts &opts);
    Word layout_word(const font::Unicode &codepoint, const size_t start, const size_t end,
                     const math::Vector2 &offset);

  public:
    TextFormatter() = default;

    TextFormatter(font::FontLoader *font_loader) : m_font_loader(font_loader) {};

    TextFormatter(const TextFormatter &) = delete;
    TextFormatter(TextFormatter &&) noexcept = default;
    TextFormatter &operator=(const TextFormatter &) = delete;
    TextFormatter &operator=(TextFormatter &&other) noexcept {
        m_font_loader = other.m_font_loader;
        return *this;
    }

    void set_font_loader(FontLoader *font_loader) { m_font_loader = font_loader; }
    TextLayout format(const font::Unicode &codepoint, const TextOpts &opts);
};

} // namespace font
