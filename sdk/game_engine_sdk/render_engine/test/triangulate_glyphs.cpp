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

        const std::vector<std::array<size_t, 3>> triangles =
            triangulation::earcut(outline);
        std::cout << std::endl;

        ASSERT_EQ(triangles.size(), outline.size() - 2);
        count++;
    }
    ASSERT_TRUE(count > 0);
}
