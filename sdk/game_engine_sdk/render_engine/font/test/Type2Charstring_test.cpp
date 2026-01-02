#include "font/OTFFont.h"
#include <gtest/gtest.h>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

TEST(Type2CharstringTests, Test_ParseGlyphC) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    const font::Glyph glyph = otf_font.glyphs[otf_font.glyph_index(font::Unicode("C"))];

    EXPECT_TRUE(glyph.curves.size() > 1);
    EXPECT_EQ(glyph.polygons.size(), 1);
    EXPECT_TRUE(glyph.polygons[0].exterior_outline.size() > 1);
    EXPECT_EQ(glyph.polygons[0].interior_outlines.size(), 0);
}

TEST(Type2CharstringTests, Test_ParseGlyphA) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    const font::Glyph glyph = otf_font.glyphs[otf_font.glyph_index(font::Unicode("A"))];

    EXPECT_TRUE(glyph.curves.size() == 0);
    EXPECT_EQ(glyph.polygons.size(), 1);
    EXPECT_TRUE(glyph.polygons[0].exterior_outline.size() == 8);
    EXPECT_EQ(glyph.polygons[0].interior_outlines.size(), 1);
    EXPECT_TRUE(glyph.polygons[0].interior_outlines[0].size() == 3);
}

TEST(Type2CharstringTests, Test_ParseGlyphD) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    const font::Glyph glyph = otf_font.glyphs[otf_font.glyph_index(font::Unicode("D"))];

    EXPECT_TRUE(glyph.curves.size() > 1);
    EXPECT_EQ(glyph.polygons.size(), 1);
    EXPECT_TRUE(glyph.polygons[0].exterior_outline.size() > 2);
    EXPECT_EQ(glyph.polygons[0].interior_outlines.size(), 1);
    EXPECT_TRUE(glyph.polygons[0].interior_outlines[0].size() > 2);
}

TEST(Type2CharstringTests, Test_ParseGlyphB) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    const font::Glyph glyph = otf_font.glyphs[otf_font.glyph_index(font::Unicode("B"))];

    EXPECT_TRUE(glyph.curves.size() > 1);
    EXPECT_EQ(glyph.polygons.size(), 1);
    EXPECT_TRUE(glyph.polygons[0].exterior_outline.size() > 1);
    EXPECT_EQ(glyph.polygons[0].interior_outlines.size(), 2);
}

TEST(Type2CharstringTests, Test_ParseGlyphi) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    const font::Glyph glyph = otf_font.glyphs[otf_font.glyph_index(font::Unicode("i"))];

    EXPECT_TRUE(glyph.curves.size() > 1);
    EXPECT_EQ(glyph.polygons.size(), 2);

    EXPECT_TRUE(glyph.polygons[0].exterior_outline.size() > 1);
    EXPECT_EQ(glyph.polygons[0].interior_outlines.size(), 0);

    EXPECT_TRUE(glyph.polygons[1].exterior_outline.size() > 1);
    EXPECT_EQ(glyph.polygons[1].interior_outlines.size(), 0);
}
