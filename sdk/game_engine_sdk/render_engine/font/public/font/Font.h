#pragma once

#include "font/FontLoader.h"
#include "font/TextFormatter.h"
#include "font/types.h"
#include <string>

namespace font {

// TODO: Make private
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

    std::vector<size_t> m_first_index;
    std::vector<size_t> m_index_count;
    /*std::vector<size_t> m_vertex_offset;*/

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
            m_first_index = std::move(other.m_first_index);
            m_index_count = std::move(other.m_index_count);
            /*m_vertex_offset = std::move(other.m_vertex_offset);*/
        }
        return *this;
    }

    size_t get_first_index(const char32_t &c);
    size_t get_index_count(const char32_t &c);
    /*size_t get_vertex_offset(const char32_t &c);*/

    TextFormat create_text_format(const TextOpts &opts);
    Text create_text(const font::Unicode &codepoint, const TextOpts &opts);

    signed long get_num_glyphs() const;

    FontBBox get_font_bbox() const;

    unsigned short get_units_per_em() const;

    float get_adjusted_font_size(float font_size);

    bool is_loaded() const { return !vertices.empty(); }
};

} // namespace font
