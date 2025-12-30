#include "font/OTFFont.h"
#include "triangulation/earcut.h"
#include <gtest/gtest.h>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

TEST(TriangulateGlyphsTest, Test_TriangulateAllGlyphs) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    size_t count = 0;
    for (const auto &glyph : otf_font.glyphs) {
        const auto &outline = glyph.vertices[0].interior;

        if (outline.size() < 3) {
            continue;
        }

        std::cout << glyph.name << ": ";
        for (const auto &point : outline) {
            std::cout << std::format("({},{}) ", point.first, point.second);
        }
        std::cout << std::endl;

        const std::vector<std::array<size_t, 3>> triangles =
            triangulation::Earcut<float>::run(outline, {});

        ASSERT_EQ(triangles.size(), outline.size() - 2);
        count++;
    }
    ASSERT_TRUE(count > 0);
}

TEST(TriangulateGlyphsTest, DISABLED_Test_TriangulateGlyph) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    const uint16_t index = otf_font.glyph_index(font::Unicode("2"));
    const font::Glyph glyph = otf_font.glyphs[index];
    const auto &outline = glyph.vertices[0].interior;

    std::cout << glyph.name << ": ";
    for (const auto &point : outline) {
        std::cout << std::format("({},{}) ", point.first, point.second);
    }
    std::cout << std::endl;

    const std::vector<std::array<size_t, 3>> triangles =
        triangulation::Earcut<float>::run(outline, {});

    ASSERT_EQ(triangles.size(), outline.size() - 2);
}
