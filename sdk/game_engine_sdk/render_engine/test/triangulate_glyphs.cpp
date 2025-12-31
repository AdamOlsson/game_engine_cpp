#include "font/OTFFont.h"
#include "triangulation/earcut.h"
#include <gtest/gtest.h>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

TEST(TriangulateGlyphsTest, DISABLED_Test_TriangulateAllGlyphs) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    size_t count = 0;
    for (const auto &glyph : otf_font.glyphs) {

        const font::Polygon &polygon = glyph.polygons[0];

        if (polygon.exterior_outline.size() == 0) {
            continue;
        }

        const font::Outline<font::Vertex<float>> &outline = polygon.exterior_outline;

        font::Outline<font::Vertex<float>> hole = {};
        if (polygon.interior_outlines.size() > 0) {
            hole = polygon.interior_outlines[0];
        }

        const std::vector<std::array<size_t, 3>> triangles =
            triangulation::Earcut<float>::run(outline, hole);

        const size_t additional = hole.size() > 0 ? 1 : 0;
        ASSERT_EQ(triangles.size(), outline.size() + hole.size() - 2 + additional);
        count++;
    }
    ASSERT_TRUE(count > 0);
}

TEST(TriangulateGlyphsTest, DISABLED_Test_TriangulateGlyph) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    const uint16_t index = otf_font.glyph_index(font::Unicode("&"));
    const font::Glyph glyph = otf_font.glyphs[index];

    const font::Polygon &polygon = glyph.polygons[0];

    const font::Outline<font::Vertex<float>> &outline = polygon.exterior_outline;

    font::Outline<font::Vertex<float>> hole = {};
    if (polygon.interior_outlines.size() > 0) {
        hole = polygon.interior_outlines[0];
    }

    const std::vector<std::array<size_t, 3>> triangles =
        triangulation::Earcut<float>::run(outline, hole);
    const size_t additional = hole.size() > 0 ? 1 : 0;
    ASSERT_EQ(triangles.size(), outline.size() + hole.size() - 2 + additional);
}
