#include "font/OTFFont.h"
#include "triangulation/Triangles.h"
#include "triangulation/earcut.h"
#include <gtest/gtest.h>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

TEST(TriangulateGlyphsTest, Test_TriangulateAllGlyphsInRabbidHighway) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    size_t count = 0;
    for (const auto &glyph : otf_font.glyphs) {

        for (const font::Polygon &polygon : glyph.polygons) {

            if (polygon.exterior_outline.size() == 0) {
                continue;
            }

            const triangulation::Triangles<float> triangles =
                triangulation::Earcut<float>::run(polygon.exterior_outline,
                                                  polygon.interior_outlines);

            int expected_number_of_indices = polygon.exterior_outline.size() - 2;
            for (const auto &internal_outline : polygon.interior_outlines) {
                expected_number_of_indices += (internal_outline.size() + 2);
            }

            ASSERT_EQ(triangles.indices.size(), expected_number_of_indices);
            count++;
        }
    }
    ASSERT_TRUE(count > 0);
}

TEST(TriangulateGlyphsTest, Test_TriangulateAllGlyphsInFtyStrategyCID) {
    const font::OTFFont otf_font = font::OTFFont(ASSET_FILE("ftystrategycidencv.otf"));

    size_t count = 0;
    for (const auto &glyph : otf_font.glyphs) {

        for (const font::Polygon &polygon : glyph.polygons) {

            if (polygon.exterior_outline.size() == 0) {
                continue;
            }

            const triangulation::Triangles<float> triangles =
                triangulation::Earcut<float>::run(polygon.exterior_outline,
                                                  polygon.interior_outlines);

            int expected_number_of_indices = polygon.exterior_outline.size() - 2;
            for (const auto &internal_outline : polygon.interior_outlines) {
                expected_number_of_indices += (internal_outline.size() + 2);
            }

            ASSERT_EQ(triangles.indices.size(), expected_number_of_indices);
            count++;
        }
    }
    ASSERT_TRUE(count > 0);
}

TEST(TriangulateGlyphsTest, Test_TriangulateGlyph) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    const uint16_t index = otf_font.glyph_index(font::Unicode("%"));
    const font::Glyph glyph = otf_font.glyphs[index];

    for (const font::Polygon &polygon : glyph.polygons) {

        const triangulation::Triangles<float> triangles =
            triangulation::Earcut<float>::run(polygon.exterior_outline,
                                              polygon.interior_outlines);

        int expected_number_of_indices = polygon.exterior_outline.size() - 2;
        for (const std::vector<std::pair<float, float>> &outline :
             polygon.interior_outlines) {
            expected_number_of_indices += (outline.size() + 2);
        }

        ASSERT_EQ(triangles.indices.size(), expected_number_of_indices);
    }
}
