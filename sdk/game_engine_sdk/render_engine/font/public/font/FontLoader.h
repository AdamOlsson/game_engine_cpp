#pragma once

#include "font/FontFormat.h"
#include "font/Unicode.h"
#include <string>

#include <ft2build.h>
#include <vector>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace font {

struct GlyphOutlines {
    std::vector<std::vector<std::pair<float, float>>> line_segments;
    std::vector<std::vector<std::array<std::pair<float, float>, 3>>> quadratic_curves;
};

class FontLoader {
  public:
    FontLoader(const std::string &filepath);
    FontLoader(const std::vector<char> &font_data);
    FontLoader(const FontLoader &) = delete;
    FontLoader &operator=(const FontLoader &) = delete;

    FontLoader(FontLoader &&other) noexcept;
    FontLoader &operator=(FontLoader &&other) noexcept;

    ~FontLoader();

    class GlyphIterator {
      public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = unsigned int; // Just the glyph index
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type *;
        using reference = const value_type &;

        GlyphIterator(FT_Face face, FT_UInt glyph_index)
            : m_face(face), m_glyph_index(glyph_index) {}

        value_type operator*() const { return m_glyph_index; }

        GlyphIterator &operator++() {
            ++m_glyph_index;
            return *this;
        }

        GlyphIterator operator++(int) {
            GlyphIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const GlyphIterator &other) const {
            return m_face == other.m_face && m_glyph_index == other.m_glyph_index;
        }

        bool operator!=(const GlyphIterator &other) const { return !(*this == other); }

      private:
        FT_Face m_face;
        FT_UInt m_glyph_index;
    };

    GlyphIterator begin() const { return GlyphIterator(m_face, 0); }
    GlyphIterator end() const {
        return GlyphIterator(m_face, m_face ? m_face->num_glyphs : 0);
    }

    unsigned int get_glyph_index(const font::Unicode &codepoint) const;
    std::array<char, 256> get_glyph_name(const font::Unicode &codepoint) const;
    std::array<char, 256> get_glyph_name(const unsigned int gid) const;
    GlyphOutlines get_glyph_outline(const font::Unicode &codepoint) const;
    GlyphOutlines get_glyph_outline(const unsigned int gid) const;
    signed long get_num_glyphs();
    FontFormat get_format();

  private:
    FT_Library m_library;
    FT_Face m_face;
    FontFormat m_format;
};

} // namespace font
