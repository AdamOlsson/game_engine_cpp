
#include "math/Vector3.h"
#include <gtest/gtest.h>

TEST(Vector3Tests, Test_AxisReference) {
    math::Vector3 vec = math::Vector3(0.0f, 0.0f, 0.0f);
    EXPECT_EQ(vec.x(), 0.0f);
    EXPECT_EQ(vec.y(), 0.0f);
    EXPECT_EQ(vec.z(), 0.0f);

    vec.x() = 1.0f;
    vec.y() = 2.0f;
    vec.z() = 3.0f;

    EXPECT_EQ(vec.x(), 1.0f);
    EXPECT_EQ(vec.y(), 2.0f);
    EXPECT_EQ(vec.z(), 3.0f);
}

TEST(Vector3Tests, Test_Operators) {
    math::Vector3 a(1.0f, 2.0f, 3.0f);
    math::Vector3 b(5.0f, 6.0f, 7.0f);

    math::Vector3 sum = a + b;
    EXPECT_EQ(sum.x(), 6.0f);
    EXPECT_EQ(sum.y(), 8.0f);
    EXPECT_EQ(sum.z(), 10.0f);

    math::Vector3 diff = b - a;
    EXPECT_EQ(diff.x(), 4.0f);
    EXPECT_EQ(diff.y(), 4.0f);
    EXPECT_EQ(diff.z(), 4.0f);

    math::Vector3 neg = -a;
    EXPECT_EQ(neg.x(), -1.0f);
    EXPECT_EQ(neg.y(), -2.0f);
    EXPECT_EQ(neg.z(), -3.0f);
}

TEST(Vector3Tests, Test_CompoundAssignment) {
    math::Vector3 a(1.0f, 2.0f, 3.0f);
    math::Vector3 b(5.0f, 6.0f, 7.0f);

    a += b;
    EXPECT_EQ(a.x(), 6.0f);
    EXPECT_EQ(a.y(), 8.0f);
    EXPECT_EQ(a.z(), 10.0f);

    a -= b;
    EXPECT_EQ(a.x(), 1.0f);
    EXPECT_EQ(a.y(), 2.0f);
    EXPECT_EQ(a.z(), 3.0f);
}

TEST(Vector3Tests, Test_ScalarMultiplication) {
    math::Vector3 a(1.0f, 2.0f, 3.0f);

    math::Vector3 scaled1 = a * 2.0f;
    EXPECT_EQ(scaled1.x(), 2.0f);
    EXPECT_EQ(scaled1.y(), 4.0f);
    EXPECT_EQ(scaled1.z(), 6.0f);

    math::Vector3 scaled2 = 3.0f * a;
    EXPECT_EQ(scaled2.x(), 3.0f);
    EXPECT_EQ(scaled2.y(), 6.0f);
    EXPECT_EQ(scaled2.z(), 9.0f);
}

TEST(Vector3Tests, Test_ScalarDivision) {
    math::Vector3 a(2.0f, 4.0f, 6.0f);

    math::Vector3 divided = a / 2.0f;
    EXPECT_EQ(divided.x(), 1.0f);
    EXPECT_EQ(divided.y(), 2.0f);
    EXPECT_EQ(divided.z(), 3.0f);
}

TEST(Vector3Tests, Test_GlmInterop) {
    math::Vector3 a(1.0f, 2.0f, 3.0f);
    glm::vec3 b(5.0f, 6.0f, 7.0f);

    math::Vector3 sum1 = a + b;
    EXPECT_EQ(sum1.x(), 6.0f);
    EXPECT_EQ(sum1.y(), 8.0f);
    EXPECT_EQ(sum1.z(), 10.0f);

    math::Vector3 sum2 = b + a;
    EXPECT_EQ(sum2.x(), 6.0f);
    EXPECT_EQ(sum2.y(), 8.0f);
    EXPECT_EQ(sum2.z(), 10.0f);

    math::Vector3 diff1 = a - b;
    EXPECT_EQ(diff1.x(), -4.0f);
    EXPECT_EQ(diff1.y(), -4.0f);
    EXPECT_EQ(diff1.z(), -4.0f);

    math::Vector3 diff2 = b - a;
    EXPECT_EQ(diff2.x(), 4.0f);
    EXPECT_EQ(diff2.y(), 4.0f);
    EXPECT_EQ(diff2.z(), 4.0f);
}

TEST(Vector3Tests, Test_GlmConversion) {
    math::Vector3 a(1.0f, 2.0f, 3.0f);
    glm::vec3 b = static_cast<glm::vec3>(a);

    EXPECT_EQ(b.x, 1.0f);
    EXPECT_EQ(b.y, 2.0f);
    EXPECT_EQ(b.z, 3.0f);
}

TEST(Vector3Tests, Test_Rotation) {
    math::Vector3 vec(1.0f, 0.0f, 0.0f);

    vec.rotate_z(90.0f);

    EXPECT_NEAR(vec.x(), 0.0f, 1e-6);
    EXPECT_NEAR(vec.y(), 1.0f, 1e-6);
    EXPECT_EQ(vec.z(), 0.0f);
}

TEST(Vector3Tests, Test_Constructors) {
    math::Vector3 a(1.0f, 2.0f, 3.0f);
    EXPECT_EQ(a.x(), 1.0f);
    EXPECT_EQ(a.y(), 2.0f);
    EXPECT_EQ(a.z(), 3.0f);

    math::Vector3 b(5.0f);
    EXPECT_EQ(b.x(), 5.0f);
    EXPECT_EQ(b.y(), 5.0f);
    EXPECT_EQ(b.z(), 5.0f);

    math::Vector2 vec2(1.0f, 2.0f);
    math::Vector3 c(vec2, 3.0f);
    EXPECT_EQ(c.x(), 1.0f);
    EXPECT_EQ(c.y(), 2.0f);
    EXPECT_EQ(c.z(), 3.0f);

    math::Vector3 d(1.0f, vec2);
    EXPECT_EQ(d.x(), 1.0f);
    EXPECT_EQ(d.y(), 1.0f);
    EXPECT_EQ(d.z(), 2.0f);
}
