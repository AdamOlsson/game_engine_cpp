#include "font/OTFFont.h"
#include <gtest/gtest.h>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

TEST(TriangulateGlyphsTest, Test_TriangulateAllGlyphs) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    // TODO: Figure out why the character C is not decoded properly
    for (const auto &glyph : otf_font.glyphs) {
        const auto &outline = glyph.outlines[0];

        std::cout << glyph.name << ": ";
        for (const auto &point : outline) {
            std::cout << std::format("({},{}) ", point.first, point.second);
        }
        std::cout << std::endl;
    }

    EXPECT_TRUE(false);
}
