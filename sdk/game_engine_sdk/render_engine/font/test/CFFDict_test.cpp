#include "font/detail/otf_font/cff/CFFDict.h"
#include <gtest/gtest.h>

TEST(CFFDictTest, Test_ParseRealNumber) {

    float expected_number;
    std::vector<int> encoded_real_number{};
    float result;

    // According to the specification
    encoded_real_number = {0x1e, 0xe2, 0xa2, 0x5f};
    expected_number = -2.25f;
    result = font::detail::otf_font::cff::CFFDict::parse_real_number(encoded_real_number);
    EXPECT_NEAR(expected_number, result, 1e-6);

    encoded_real_number = {0x1e, 0x0a, 0x14, 0x05, 0x41, 0xc3, 0xff};
    expected_number = 0.140541e-3;
    result = font::detail::otf_font::cff::CFFDict::parse_real_number(encoded_real_number);
    EXPECT_NEAR(expected_number, result, 1e-6);
}

TEST(CFFDictTest, Test_ParseDelta) {
    std::vector<int> expected_output;
    std::vector<int> delta;
    std::vector<int> result;

    delta = {0, 1, 3, 5, 8};
    expected_output = {0, 1, 2, 2, 3};
    result = font::detail::otf_font::cff::CFFDict::parse_delta(delta);

    EXPECT_EQ(expected_output.size(), result.size());
    EXPECT_EQ(expected_output, result);
}
