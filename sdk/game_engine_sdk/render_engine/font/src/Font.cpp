#include "font/Font.h"
#include "font/Polygon.h"
#include "math/winding.h"
#include "triangulation/mapbox/earcut.h"

namespace font {

Font::Font(const std::string &font_path)
    : m_loader(FontLoader(font_path)), m_formatter(&m_loader) {
    triangulate_glyphs();
}

TextFormat Font::create_text_format(const TextOpts &opts) const {
    return {
        .position = math::Vector3(opts.position, 0.0f),
        .font_size = get_adjusted_font_size(opts.font_size),
        .font_color = opts.font_color,
    };
}

Text Font::create_text(const font::Unicode &codepoint, const TextOpts &opts) const {

    TextLayout layout = m_formatter.format(codepoint, opts);
    const float font_scale = opts.font_size / m_loader.get_units_per_em();

    Text text{};
    text.glyphs.reserve(layout.char_count);
    text.bbox = layout.bbox;
    text.bbox.offset(opts.position);

    for (const font::Word &word : layout.words) {
        const size_t num_chars_in_word = word.end_idx - word.start_idx;

        for (size_t char_idx = 0; char_idx < num_chars_in_word; char_idx++) {

            const char32_t &c = codepoint[word.start_idx + char_idx];

            const math::Vector2 &char_offset = word.glyph_positions[char_idx];

            Glyph glyph{};
            glyph.unicode_char = c;
            glyph.index_count = get_index_count(c);
            glyph.first_index = get_first_index(c);
            glyph.offset = (word.offset + char_offset) * font_scale;

            text.glyphs.push_back(glyph);
        }
    }

    return text;
}

float Font::get_adjusted_font_size(float font_size) const {
    return font_size / m_loader.get_units_per_em();
}

size_t Font::get_first_index(const char32_t &c) const {
    return m_first_index[m_loader.get_glyph_index(c)];
}

size_t Font::get_index_count(const char32_t &c) const {
    return m_index_count[m_loader.get_glyph_index(c)];
}

/*size_t Font::get_vertex_offset(const char32_t &c) {*/
/*    return m_vertex_offset[m_loader.get_glyph_index(c)];*/
/*}*/

signed long Font::get_num_glyphs() const { return m_loader.get_num_glyphs(); }

FontBBox Font::get_font_bbox() const { return m_loader.get_font_bbox(); }

unsigned short Font::get_units_per_em() const { return m_loader.get_units_per_em(); }

void Font::triangulate_glyphs() {

    /*m_glyph_draw_info.reserve(m_loader.get_num_glyphs());*/

    m_first_index.reserve(m_loader.get_num_glyphs());
    m_index_count.reserve(m_loader.get_num_glyphs());
    /*m_vertex_offset.reserve(m_loader.get_num_glyphs());*/

    // class memebers
    /*std::vector<GlyphVertex> vertices;*/
    /*std::vector<uint16_t> indices;*/

    for (const auto glyph_index : m_loader) {
        font::GlyphOutlines outlines = m_loader.get_glyph_outline(glyph_index);
        const font::FontFill exterior_fill = outlines.fill;

        const size_t first_index = indices.size();
        const size_t vertex_offset = vertices.size();
        if (!outlines.line_segments.empty()) {
            const std::vector<Polygon> glyph_polygons =
                Polygon::construct_polygons(outlines);

            for (const Polygon polygon : glyph_polygons) {
                const size_t first_vertex = vertices.size();

                const std::vector<unsigned int> triangle_indices =
                    mapbox::earcut(polygon.get_outlines());

                // First load write all vertices from earcut to the vertex buffer
                for (const std::vector<std::pair<float, float>> &outline :
                     polygon.get_outlines()) {
                    for (const std::pair<float, float> &vertex : outline) {
                        vertices.emplace_back(vertex.first, vertex.second, 0.0f, 0.0f,
                                              0.0f, 0.0f);
                    }
                }

                // Secondly write the indices forming the triangles from earcut into
                // to index buffer
                for (const unsigned int index : triangle_indices) {
                    indices.emplace_back(first_vertex + index);
                }

                // Thirdly write all curve segments into vertex and index buffer
                // (which by nature of bezier curves are already triangulated)
                const auto quad_curves = polygon.get_quadratic_curves();
                for (size_t outline_index = 0; outline_index < quad_curves.size();
                     outline_index++) {

                    const std::vector<std::array<std::pair<float, float>, 3>> &outline =
                        quad_curves[outline_index];

                    for (const std::array<std::pair<float, float>, 3> &curve : outline) {
                        const bool ccw = math::is_counter_clockwise_winding(curve);

                        const bool wants_right_fill =
                            (exterior_fill == font::FontFill::Right);
                        const float winding_order =
                            (wants_right_fill == !ccw) ? 1.0f : -1.0f;

                        indices.emplace_back(vertices.size());
                        vertices.emplace_back(curve[0].first, curve[0].second,
                                              winding_order, 0.0f, 0.0f, 1.0f);

                        indices.emplace_back(vertices.size());
                        vertices.emplace_back(curve[1].first, curve[1].second,
                                              winding_order, 0.5f, 0.0f, 1.0f);

                        indices.emplace_back(vertices.size());
                        vertices.emplace_back(curve[2].first, curve[2].second,
                                              winding_order, 1.0f, 1.0f, 1.0f);
                    }
                }
            }
        }

        const size_t count = indices.size() - first_index;
        /*m_glyph_draw_info.emplace_back(count, first_index);*/
        m_first_index.push_back(first_index);
        m_index_count.push_back(count);
        /*m_vertex_offset.push_back(vertex_offset);*/
    }
}
}; // namespace font
