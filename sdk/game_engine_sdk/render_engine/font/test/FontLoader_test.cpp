

#include "font/FontLoader.h"

#include <gtest/gtest.h>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

TEST(FontLoaderTests, DISABLED_Test_Dummy) {
    font::FontLoader loader =
        font::FontLoader(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));
    /*font::FontLoader(ASSET_FILE("TypeLightSans-KV84p.otf"));*/

    auto glyph_outlines = loader.get_glyph_outline(font::Unicode("I"));

    EXPECT_FALSE(true);
}
