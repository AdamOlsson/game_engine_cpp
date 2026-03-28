#pragma once

#include "font/FontLoader.h"
#include "font/TextFormatter.h"
#include <string>

namespace font {

// TODO: Keep private
struct GlyphVertex {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float u = 0.0f;
    float v = 0.0f;
    float w = 0.0f;
};

class Font {
  private:
    FontLoader m_loader;
    TextFormatter m_formatter;

    std::vector<std::pair<size_t, size_t>> m_glyph_draw_info;

    void triangulate_glyphs();

  public:
    // Temporarily public
    std::vector<GlyphVertex> vertices;
    std::vector<uint16_t> indices;

    Font() = default;

    Font(const std::string &font_path);

    Font(const Font &) = delete;
    Font(Font &&) noexcept = default;
    Font &operator=(const Font &) = delete;
    Font &operator=(Font &&other) noexcept {
        if (this != &other) {
            m_loader = std::move(other.m_loader);
            m_formatter = std::move(other.m_formatter);
            m_formatter.set_font_loader(&m_loader);
            vertices = std::move(other.vertices);
            indices = std::move(other.indices);
            m_glyph_draw_info = std::move(other.m_glyph_draw_info);
        }
        return *this;
    }

    std::pair<size_t, size_t> get_draw_info(const char32_t &c);
    Text format(const font::Unicode &codepoint, const TextOpts &opts);
    signed long get_num_glyphs() const;
    FontBBox get_font_bbox() const;
    unsigned short get_units_per_em() const;
    const std::pair<size_t, size_t> &get_glyph_draw_info(size_t glyph_index) const;
    bool is_loaded() const { return !vertices.empty(); }
};

} // namespace font
