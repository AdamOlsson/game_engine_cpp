#include "font/Font.h"
#include "font/Polygon.h"
#include "math/winding.h"
#include "triangulation/mapbox/earcut.h"

namespace font {

Font::Font(const std::string &font_path)
    : m_loader(FontLoader(font_path)), m_formatter(&m_loader) {
    triangulate_glyphs();
}

Text Font::format(const font::Unicode &codepoint, const TextOpts &opts) {
    return m_formatter.format(codepoint, opts);
}

std::pair<size_t, size_t> Font::get_draw_info(const char32_t &c) {
    return m_glyph_draw_info[m_loader.get_glyph_index(c)];
}

signed long Font::get_num_glyphs() const { return m_loader.get_num_glyphs(); }

FontBBox Font::get_font_bbox() const { return m_loader.get_font_bbox(); }

unsigned short Font::get_units_per_em() const { return m_loader.get_units_per_em(); }

const std::pair<size_t, size_t> &Font::get_glyph_draw_info(size_t glyph_index) const {
    return m_glyph_draw_info[glyph_index];
}

void Font::triangulate_glyphs() {

    m_glyph_draw_info.reserve(m_loader.get_num_glyphs());

    // class memebers
    /*std::vector<GlyphVertex> vertices;*/
    /*std::vector<uint16_t> indices;*/

    for (const auto glyph_index : m_loader) {
        font::GlyphOutlines outlines = m_loader.get_glyph_outline(glyph_index);
        const font::FontFill exterior_fill = outlines.fill;

        const size_t first_index = indices.size();
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
        m_glyph_draw_info.emplace_back(count, first_index);
    }
}
}; // namespace font
