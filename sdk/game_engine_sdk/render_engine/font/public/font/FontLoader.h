#pragma once

#include "font/FontFormat.h"
#include "font/Unicode.h"
#include <sstream>
#include <string>

#include <ft2build.h>
#include <vector>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

namespace font {

struct GlyphMetrics {
    signed long width = 0;
    signed long height = 0;

    signed long hori_bearing_x = 0;
    signed long hori_bearing_y = 0;
    signed long hori_advance = 0;

    signed long vert_bearing_x = 0;
    signed long vert_bearing_y = 0;
    signed long vert_advance = 0;
};

struct GlyphAdvance {
    signed long x = 0;
    signed long y = 0;
};

struct GlyphKerning {
    signed long x = 0;
    signed long y = 0;
};

struct FontBBox {
    int x_min = 0;
    int y_min = 0;
    int x_max = 0;
    int y_max = 0;

    std::string to_string() const {
        std::ostringstream oss;
        oss << "FontBBox{x_min:" << x_min << ", y_min:" << y_min << ", x_max:" << x_max
            << ", y_max:" << y_max << "}";
        return oss.str();
    }
};

struct GlyphOutlines {
    std::vector<std::vector<std::pair<float, float>>> line_segments;
    std::vector<std::vector<std::array<std::pair<float, float>, 3>>> quadratic_curves;
    FontFill fill;
};

class FontLoader {
  private:
    FT_Library m_library;
    FT_Face m_face;
    FontFormat m_format;

  public:
    FontLoader() = default;
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

    GlyphMetrics get_glyph_metrics(const font::Unicode &codepoint) const;
    GlyphMetrics get_glyph_metrics(const char32_t gid) const;
    GlyphMetrics get_glyph_metrics(const unsigned int gid) const;

    GlyphAdvance get_glyph_advance(const font::Unicode &codepoint) const;
    GlyphAdvance get_glyph_advance(const char32_t gid) const;
    GlyphAdvance get_glyph_advance(const unsigned int gid) const;

    GlyphKerning get_glyph_kerning(const font::Unicode &codepoint1,
                                   const font::Unicode &codepoint2) const;
    GlyphKerning get_glyph_kerning(const char32_t gid1, const char32_t gid2) const;
    GlyphKerning get_glyph_kerning(const unsigned int gid1,
                                   const unsigned int gid2) const;

    signed long get_num_glyphs() const;
    FontFormat get_format() const;
    FontBBox get_font_bbox() const;
    unsigned short get_units_per_em() const;

  private:
};

} // namespace font

std::ostream &operator<<(std::ostream &os, const font::FontBBox &bbox);
