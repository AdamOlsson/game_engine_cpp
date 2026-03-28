#include "font/TextFormatter.h"
#include "math/Bbox.h"
#include "util/assert.h"

namespace font {

TextLayout TextFormatter::format(const font::Unicode &codepoint, const TextOpts &opts) {
    DEBUG_ASSERT(m_font_loader != nullptr,
                 "Error: Trying to format code when the font loader is not set.");
    return layout_text(codepoint, opts);
}

TextLayout TextFormatter::layout_text(const font::Unicode &codepoint,
                                      const TextOpts &opts) {
    /* line_height_top ---------------- < bbox top >
     * font_padding        <padding>
     * font_bbox_top   ---------------- < word bbox >
     *                     <glyph>
     * font_bbox_bot   ---------------- < word bbox >
     * font_padding        <padding>
     * line_height_top ---------------- < bbox bot >
     * */

    const unsigned short units_per_em = m_font_loader->get_units_per_em();
    const font::FontBBox font_bbox = m_font_loader->get_font_bbox();
    const math::Vector2 font_bbox_size = math::Vector2(font_bbox.x_max - font_bbox.x_min,
                                                       font_bbox.y_max - font_bbox.y_min);

    const float font_scale = opts.font_size / units_per_em;
    const float line_width = opts.line_width / font_scale;
    const float line_height = fmax(opts.line_height / font_scale, font_bbox_size.y());
    const float line_padding = fmax(0.0f, line_height - font_bbox_size.y());

    // Identify start and stop for all words
    std::vector<std::pair<size_t, size_t>> words;
    size_t word_start = 0;
    size_t word_end = std::numeric_limits<size_t>::max();
    for (size_t i = 0; i < codepoint.size(); i++) {
        const char32_t &c = codepoint[i];
        const bool is_last = i == codepoint.size() - 1;
        if (c == SPACE) {
            word_end = i;
            words.push_back({word_start, word_end});
            word_start = i + 1;
            word_end = std::numeric_limits<size_t>::max();
        } else if (c == LF) {
            // end the word before the \n
            word_end = i;
            words.push_back({word_start, word_end});

            // append \n as its own word
            word_start = i;
            word_end = i + 1;
            words.push_back({word_start, word_end});

            word_start = i + 1;
            word_end = std::numeric_limits<size_t>::max();
        } else if (is_last) {
            word_end = i + 1;
            words.push_back({word_start, word_end});
        }
    }

    const font::GlyphAdvance &_space_advance =
        m_font_loader->get_glyph_advance(font::Unicode(SPACE));
    const math::Vector2 space_advance = math::Vector2(_space_advance.x, (signed long)0);

    TextLayout text{};
    text.words.reserve(words.size());

    math::Vector2 pen_position = math::Vector2(0, 0);

    float line_count = 0.0f;
    math::Vector2 bbox_top_left = math::Vector2(0.0f, 0.0f);
    math::Vector2 bbox_bot_right = math::Vector2(0.0f, 0.0f);

    for (size_t word_id = 0; word_id < words.size(); word_id++) {
        const size_t word_start = words[word_id].first;
        const size_t word_end = words[word_id].second;
        Word word = layout_word(codepoint, word_start, word_end, pen_position);

        if (word.is_control_char && codepoint[word.start_idx] == LF) {
            line_count++;
            pen_position = math::Vector2(0.0f, line_height * line_count);
            word.offset = pen_position;
            continue;
        }

        // newline text
        const bool exceeds_line_width = pen_position.x() + word.advance.x() > line_width;
        if (exceeds_line_width) {
            line_count++;
            pen_position = math::Vector2(0.0f, line_height * line_count);
            word.offset = pen_position;
        }

        word.offset.y() += line_padding;

        // Find the bbox of the text
        bbox_top_left.y() = -fmax(bbox_top_left.y(), word.bbox.y());
        bbox_bot_right.x() = fmax(bbox_top_left.x(), word.bbox.z() + word.offset.x());
        bbox_bot_right.y() = fmax(bbox_top_left.y(), word.bbox.w() + word.offset.y());

        pen_position += word.advance;
        pen_position += space_advance;

        text.char_count += word.glyph_positions.size();
        text.words.push_back(std::move(word));
    }

    text.bbox = math::Bbox(bbox_top_left * font_scale, bbox_bot_right * font_scale);

    return text;
}

Word TextFormatter::layout_word(const font::Unicode &codepoint, const size_t start,
                                const size_t end, const math::Vector2 &offset) {

    std::vector<math::Vector2> glyph_positions;
    glyph_positions.reserve(end - start);
    math::Vector2 pen_position = math::Vector2(0, 0);
    bool is_control_char = false;
    std::vector<std::pair<char32_t, size_t>> control_characters;

    math::Vector2 bbox_top_left = math::Vector2(0.0f, 0.0f);
    math::Vector2 bbox_bot_right = math::Vector2(0.0f, 0.0f);

    for (size_t i = start; i < end; i++) {
        const char32_t &c = codepoint[i];

        if (c == LF) {
            is_control_char = true;
        }

        glyph_positions.emplace_back(pen_position);

        const font::GlyphAdvance &advance = m_font_loader->get_glyph_advance(c);
        pen_position += math::Vector2(advance.x, (signed long)0);

        const auto metrics = m_font_loader->get_glyph_metrics(c);
        bbox_bot_right.x() += metrics.hori_advance;
        bbox_top_left.y() = fmax(bbox_top_left.y(), metrics.vert_advance);

        if (i < end - 1) {
            font::GlyphKerning kerning =
                m_font_loader->get_glyph_kerning(codepoint[i], codepoint[i + 1]);
            pen_position.x() += kerning.x;
        }
    }

    return Word{
        .is_control_char = is_control_char,
        .start_idx = start,
        .end_idx = end,
        .offset = offset,
        .advance = pen_position,
        .bbox = math::Vector4(bbox_top_left, bbox_bot_right),
        .glyph_positions = std::move(glyph_positions),
    };
}

} // namespace font
