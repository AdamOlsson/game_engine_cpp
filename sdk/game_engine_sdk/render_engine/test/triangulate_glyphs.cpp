#include "font/OTFFont.h"
#include "triangulation/Triangles.h"
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
        const triangulation::Triangles<float> triangles =
            triangulation::Earcut<float>::run(outline, polygon.interior_outlines);

        const size_t additional = polygon.interior_outlines.size() > 0 ? 2 : 0;
        const size_t internal_size =
            polygon.interior_outlines.size() > 0 ? polygon.interior_outlines.size() : 0;

        int expected_number_of_indices = fmax(outline.size() - 2, 0);
        for (const auto &internal_outline : polygon.interior_outlines) {
            expected_number_of_indices += (internal_outline.size() + 2);
            break;
        }

        ASSERT_EQ(triangles.indices.size(), expected_number_of_indices);
        count++;
    }
    ASSERT_TRUE(count > 0);
}

TEST(TriangulateGlyphsTest, DISABLED_Test_TriangulateGlyph) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    const uint16_t index = otf_font.glyph_index(font::Unicode("2"));
    const font::Glyph glyph = otf_font.glyphs[index];

    const font::Polygon &polygon = glyph.polygons[0];

    const font::Outline<font::Vertex<float>> &outline = polygon.exterior_outline;
    const triangulation::Triangles<float> triangles =
        triangulation::Earcut<float>::run(outline, polygon.interior_outlines);

    int expected_number_of_indices = fmax(outline.size() - 2, 0);
    for (const auto &internal_outline : polygon.interior_outlines) {
        expected_number_of_indices += (internal_outline.size() + 2);
        break;
    }
    ASSERT_EQ(triangles.indices.size(), expected_number_of_indices);
}
