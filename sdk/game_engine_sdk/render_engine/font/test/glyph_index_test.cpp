#include "font/OTFFont.h"
#include <gtest/gtest.h>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

TEST(GlyphIndexTests, Test_IndexGlyphsRabbidHighway) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    for (char c = 'a'; c <= 'z'; c++) {
        const size_t index = otf_font.glyph_index(font::Unicode(c));
        const font::Glyph glyph = otf_font.glyphs[index];
        EXPECT_EQ(std::format("{}", c), glyph.name);
    }

    for (char c = 'A'; c <= 'Z'; c++) {
        const size_t index = otf_font.glyph_index(font::Unicode(c));
        const font::Glyph glyph = otf_font.glyphs[index];
        EXPECT_EQ(std::format("{}", c), glyph.name);
    }
}

TEST(GlyphIndexTests, DISABLED_Test_IndexGlyphsTypeLightSans) {
    const font::OTFFont otf_font = font::OTFFont(ASSET_FILE("TypeLightSans-KV84p.otf"));

    for (char c = 'a'; c <= 'z'; c++) {
        const size_t index = otf_font.glyph_index(font::Unicode(c));
        const font::Glyph glyph = otf_font.glyphs[index];
        EXPECT_EQ(std::format("{}", c), glyph.name);
    }

    for (char c = 'A'; c <= 'Z'; c++) {
        const size_t index = otf_font.glyph_index(font::Unicode(c));
        const font::Glyph glyph = otf_font.glyphs[index];
        EXPECT_EQ(std::format("{}", c), glyph.name);
    }
}
