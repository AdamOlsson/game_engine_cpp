#include "math/Matrix.h"
#include <glm/glm.hpp>
#include <gtest/gtest.h>
#include <numbers>

static constexpr float PI = std::numbers::pi_v<float>;

// ─── Identity ────────────────────────────────────────────────────────────────

TEST(MatrixTests, Test_DefaultIsIdentity) {
    math::Matrix m;
    glm::mat4 mat = static_cast<glm::mat4>(m);
    EXPECT_EQ(mat, glm::mat4(1.0f));
}

TEST(MatrixTests, Test_Scale2dIdentity) {
    math::Matrix m;
    auto s = math::Matrix::scale_2d(m);
    EXPECT_FLOAT_EQ(s.x(), 1.0f);
    EXPECT_FLOAT_EQ(s.y(), 1.0f);
}

// ─── Uniform scale ───────────────────────────────────────────────────────────

TEST(MatrixTests, Test_Scale2dUniformFloat) {
    math::Matrix m;
    m.scale(3.0f);
    auto s = math::Matrix::scale_2d(m);
    EXPECT_FLOAT_EQ(s.x(), 3.0f);
    EXPECT_FLOAT_EQ(s.y(), 3.0f);
}

TEST(MatrixTests, Test_Scale2dUniformInt) {
    math::Matrix m;
    m.scale(4);
    auto s = math::Matrix::scale_2d(m);
    EXPECT_FLOAT_EQ(s.x(), 4.0f);
    EXPECT_FLOAT_EQ(s.y(), 4.0f);
}

// ─── Non-uniform scale ───────────────────────────────────────────────────────

TEST(MatrixTests, Test_Scale2dNonUniformXY) {
    math::Matrix m;
    m.scale(2.0f, 5.0f);
    auto s = math::Matrix::scale_2d(m);
    EXPECT_FLOAT_EQ(s.x(), 2.0f);
    EXPECT_FLOAT_EQ(s.y(), 5.0f);
}

TEST(MatrixTests, Test_Scale2dVector2) {
    math::Matrix m;
    m.scale(math::Vector2(3.0f, 7.0f));
    auto s = math::Matrix::scale_2d(m);
    EXPECT_FLOAT_EQ(s.x(), 3.0f);
    EXPECT_FLOAT_EQ(s.y(), 7.0f);
}

TEST(MatrixTests, Test_Scale2dGlmVec3) {
    math::Matrix m;
    m.scale(glm::vec3(2.0f, 4.0f, 8.0f));
    auto s = math::Matrix::scale_2d(m);
    EXPECT_FLOAT_EQ(s.x(), 2.0f);
    EXPECT_FLOAT_EQ(s.y(), 4.0f);
}

// ─── Scale + translate independence ──────────────────────────────────────────

TEST(MatrixTests, Test_Scale2dUnaffectedByTranslate) {
    math::Matrix m;
    m.scale(2.0f, 3.0f).translate(100.0f, 200.0f);
    auto s = math::Matrix::scale_2d(m);
    EXPECT_FLOAT_EQ(s.x(), 2.0f);
    EXPECT_FLOAT_EQ(s.y(), 3.0f);
}

TEST(MatrixTests, Test_Scale2dUnaffectedByTranslateOrder) {
    math::Matrix a, b;
    a.translate(50.0f, 50.0f).scale(2.0f, 3.0f);
    b.scale(2.0f, 3.0f).translate(50.0f, 50.0f);
    EXPECT_FLOAT_EQ(math::Matrix::scale_2d(a).x(), math::Matrix::scale_2d(b).x());
    EXPECT_FLOAT_EQ(math::Matrix::scale_2d(a).y(), math::Matrix::scale_2d(b).y());
}

// ─── Scale + rotation independence ───────────────────────────────────────────

TEST(MatrixTests, Test_Scale2dPreservedWhenRotateBeforeScale) {
    math::Matrix m;
    m.rotate_z(PI / 4.0f).scale(2.0f, 3.0f);
    auto s = math::Matrix::scale_2d(m);
    EXPECT_NEAR(s.x(), 2.0f, 1e-5f);
    EXPECT_NEAR(s.y(), 3.0f, 1e-5f);
}

TEST(MatrixTests, Test_Scale2dUniformPreservedAfterRotateZ) {
    math::Matrix m;
    m.scale(3.0f).rotate_z(PI / 4.0f);
    auto s = math::Matrix::scale_2d(m);
    EXPECT_NEAR(s.x(), 3.0f, 1e-5f);
    EXPECT_NEAR(s.y(), 3.0f, 1e-5f);
}

TEST(MatrixTests, Test_Scale2dInvariantAcrossRotationAngles) {
    const float angles[] = {0.0f, PI / 6, PI / 4, PI / 3, PI / 2, PI, 3 * PI / 2};
    for (float angle : angles) {
        math::Matrix m;
        m.rotate_z(angle).scale(2.0f, 5.0f); // rotate THEN scale
        auto s = math::Matrix::scale_2d(m);
        EXPECT_NEAR(s.x(), 2.0f, 1e-5f) << "Failed at angle " << angle;
        EXPECT_NEAR(s.y(), 5.0f, 1e-5f) << "Failed at angle " << angle;
    }
}

// ─── Chained operations ───────────────────────────────────────────────────────

TEST(MatrixTests, Test_Scale2dChainedWithAllTransforms) {
    math::Matrix m;
    m.translate(10.0f, 20.0f).rotate_z(PI / 3.0f).scale(4.0f, 6.0f);
    auto s = math::Matrix::scale_2d(m);
    EXPECT_NEAR(s.x(), 4.0f, 1e-5f);
    EXPECT_NEAR(s.y(), 6.0f, 1e-5f);
}

TEST(MatrixTests, Test_Scale2dCumulativeScaleMultiplies) {
    math::Matrix m;
    m.scale(2.0f, 3.0f).scale(2.0f, 2.0f);
    auto s = math::Matrix::scale_2d(m);
    EXPECT_NEAR(s.x(), 4.0f, 1e-5f);
    EXPECT_NEAR(s.y(), 6.0f, 1e-5f);
}

// ─── Edge cases ───────────────────────────────────────────────────────────────

TEST(MatrixTests, Test_Scale2dByOneIsNoop) {
    math::Matrix m;
    m.scale(1.0f, 1.0f);
    auto s = math::Matrix::scale_2d(m);
    EXPECT_FLOAT_EQ(s.x(), 1.0f);
    EXPECT_FLOAT_EQ(s.y(), 1.0f);
}

TEST(MatrixTests, Test_Scale2dSmallValue) {
    math::Matrix m;
    m.scale(0.0001f, 0.0001f);
    auto s = math::Matrix::scale_2d(m);
    EXPECT_NEAR(s.x(), 0.0001f, 1e-6f);
    EXPECT_NEAR(s.y(), 0.0001f, 1e-6f);
}

TEST(MatrixTests, Test_Scale2dLargeValue) {
    math::Matrix m;
    m.scale(10000.0f, 10000.0f);
    auto s = math::Matrix::scale_2d(m);
    EXPECT_FLOAT_EQ(s.x(), 10000.0f);
    EXPECT_FLOAT_EQ(s.y(), 10000.0f);
}

// ─── position_2d unaffected by scale ─────────────────────────────────────────

TEST(MatrixTests, Test_Position2dUnaffectedByScale) {
    math::Matrix m;
    m.translate(5.0f, 7.0f).scale(10.0f, 10.0f);
    auto p = m.position_2d();
    EXPECT_FLOAT_EQ(p.x(), 5.0f);
    EXPECT_FLOAT_EQ(p.y(), 7.0f);
}
