#include "font/OTFFont.h"
#include <gtest/gtest.h>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

TEST(Type2CharstringTests, DISABLED_Test_ParseGlyphC) {
    const font::OTFFont otf_font =
        font::OTFFont(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));

    const font::Glyph glyph = otf_font.glyphs[otf_font.glyph_index(font::Unicode("C"))];

    // TODO: Create all triangles exactly as when I do in load_font()

    EXPECT_FALSE(true);
}
