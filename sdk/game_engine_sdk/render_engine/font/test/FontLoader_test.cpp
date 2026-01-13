

#include "font/FontLoader.h"

#include <gtest/gtest.h>

#define ASSET_FILE(filename) ASSET_DIR "/" filename

TEST(FontLoaderTests, Test_Dummy) {
    font::FontLoader loader =
        /*font::FontLoader(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf"));*/
        font::FontLoader(ASSET_FILE("TypeLightSans-KV84p.otf"));

    loader.get_glyph_outline(font::Unicode("A"));

    EXPECT_FALSE(true);
}
