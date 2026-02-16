
#include "math/Vector4.h"
#include <gtest/gtest.h>

TEST(Vector4Tests, Test_AxisReference) {
    math::Vector4 vec = math::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    EXPECT_EQ(vec.x(), 0.0f);
    EXPECT_EQ(vec.y(), 0.0f);
    EXPECT_EQ(vec.z(), 0.0f);
    EXPECT_EQ(vec.w(), 0.0f);

    vec.x() = 1.0f;
    vec.y() = 2.0f;
    vec.z() = 3.0f;
    vec.w() = 4.0f;

    EXPECT_EQ(vec.x(), 1.0f);
    EXPECT_EQ(vec.y(), 2.0f);
    EXPECT_EQ(vec.z(), 3.0f);
    EXPECT_EQ(vec.w(), 4.0f);
}

TEST(Vector4Tests, Test_Operators) {
    math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    math::Vector4 b(5.0f, 6.0f, 7.0f, 8.0f);

    math::Vector4 sum = a + b;
    EXPECT_EQ(sum.x(), 6.0f);
    EXPECT_EQ(sum.y(), 8.0f);
    EXPECT_EQ(sum.z(), 10.0f);
    EXPECT_EQ(sum.w(), 12.0f);

    math::Vector4 diff = b - a;
    EXPECT_EQ(diff.x(), 4.0f);
    EXPECT_EQ(diff.y(), 4.0f);
    EXPECT_EQ(diff.z(), 4.0f);
    EXPECT_EQ(diff.w(), 4.0f);

    math::Vector4 neg = -a;
    EXPECT_EQ(neg.x(), -1.0f);
    EXPECT_EQ(neg.y(), -2.0f);
    EXPECT_EQ(neg.z(), -3.0f);
    EXPECT_EQ(neg.w(), -4.0f);
}

TEST(Vector4Tests, Test_CompoundAssignment) {
    math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    math::Vector4 b(5.0f, 6.0f, 7.0f, 8.0f);

    a += b;
    EXPECT_EQ(a.x(), 6.0f);
    EXPECT_EQ(a.y(), 8.0f);
    EXPECT_EQ(a.z(), 10.0f);
    EXPECT_EQ(a.w(), 12.0f);

    a -= b;
    EXPECT_EQ(a.x(), 1.0f);
    EXPECT_EQ(a.y(), 2.0f);
    EXPECT_EQ(a.z(), 3.0f);
    EXPECT_EQ(a.w(), 4.0f);
}

TEST(Vector4Tests, Test_GlmInterop) {
    math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    glm::vec4 b(5.0f, 6.0f, 7.0f, 8.0f);

    math::Vector4 sum1 = a + b;
    EXPECT_EQ(sum1.x(), 6.0f);
    EXPECT_EQ(sum1.y(), 8.0f);
    EXPECT_EQ(sum1.z(), 10.0f);
    EXPECT_EQ(sum1.w(), 12.0f);

    math::Vector4 sum2 = b + a;
    EXPECT_EQ(sum2.x(), 6.0f);
    EXPECT_EQ(sum2.y(), 8.0f);
    EXPECT_EQ(sum2.z(), 10.0f);
    EXPECT_EQ(sum2.w(), 12.0f);

    math::Vector4 diff1 = a - b;
    EXPECT_EQ(diff1.x(), -4.0f);
    EXPECT_EQ(diff1.y(), -4.0f);
    EXPECT_EQ(diff1.z(), -4.0f);
    EXPECT_EQ(diff1.w(), -4.0f);

    math::Vector4 diff2 = b - a;
    EXPECT_EQ(diff2.x(), 4.0f);
    EXPECT_EQ(diff2.y(), 4.0f);
    EXPECT_EQ(diff2.z(), 4.0f);
    EXPECT_EQ(diff2.w(), 4.0f);
}

TEST(Vector4Tests, Test_GlmConversion) {
    math::Vector4 a(1.0f, 2.0f, 3.0f, 4.0f);
    glm::vec4 b = static_cast<glm::vec4>(a);

    EXPECT_EQ(b.x, 1.0f);
    EXPECT_EQ(b.y, 2.0f);
    EXPECT_EQ(b.z, 3.0f);
    EXPECT_EQ(b.w, 4.0f);
}
