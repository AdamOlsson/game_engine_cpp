#include "game_engine_sdk/equations/equations.h"
#include "game_engine_sdk/equations/round.h"
#include "game_engine_sdk/io.h"
#include "glm/ext/scalar_constants.hpp"
#include "glm/glm.hpp"
#include <gtest/gtest.h>

TEST(EquationsTest, Given2dVectorExpectRoundedTo3Decimals) {
    glm::vec2 v = glm::vec2(1.2456, -0.5679);
    auto output = Round::round(v);
    glm::vec2 expected = glm::vec2(1.246, -0.568);
    EXPECT_EQ(expected, output)
        << "Expected " << io::to_string(expected) << " found " << io::to_string(output);
}

TEST(EquationsTest, RoundMut2dVectorProducesSameResultAsRound) {
    glm::vec2 v1 = glm::vec2(1.2456, -0.5679);
    glm::vec2 v2 = glm::vec2(1.2456, -0.5679);
    auto output1 = Round::round(v1);
    Round::round_mut(v2);
    EXPECT_EQ(output1, v2) << "Expected " << io::to_string(output1) << " found "
                           << io::to_string(v2);
}

TEST(EquationsTest, Given3dVectorExpectRoundedTo3Decimals) {
    glm::vec3 v = glm::vec3(1.2456, -0.5679, -1.2455);
    auto output = Round::round(v);
    glm::vec3 expected = glm::vec3(1.246, -0.568, -1.246);
    EXPECT_EQ(expected, output)
        << "Expected " << io::to_string(expected) << " found " << io::to_string(output);
}

TEST(EquationsTest, RoundMut3dVectorProducesSameResultAsRound) {
    glm::vec3 v1 = glm::vec3(1.2456, -0.5679, -1.2455);
    glm::vec3 v2 = glm::vec3(1.2456, -0.5679, -1.2455);
    auto output1 = Round::round(v1);
    Round::round_mut(v2);
    EXPECT_EQ(output1, v2) << "Expected " << io::to_string(output1) << " found "
                           << io::to_string(v2);
}

TEST(EquationsTest, GivenXAxisUnit3dVectorRotate90DegreesExpectYAxisUnitVector) {
    glm::vec3 v = glm::vec3(1.0, 0.0, 0.0);
    float degrees_90 = glm::pi<float>() / 2.0;
    glm::vec3 output = Round::round(Equations::rotate_z(v, degrees_90));
    glm::vec3 expected = glm::vec3(0.0, -1.0, 0.0);

    EXPECT_EQ(expected, output)
        << "Expected " << io::to_string(expected) << " found " << io::to_string(output);
}

TEST(EquationsTest, GivenXAxisUnit2dVectorRotate90DegreesExpectYAxisUnitVector) {
    glm::vec2 v = glm::vec2(1.0, 0.0);
    glm::vec2 output = Round::round(Equations::rotate(v, glm::radians(90.0f)));
    glm::vec2 expected = glm::vec2(0.0, -1.0);

    EXPECT_EQ(expected, output)
        << "Expected " << io::to_string(expected) << " found " << io::to_string(output);
}

TEST(EquationsTest, RotateMut2dVecShouldProduceSameResultAsRotate) {
    glm::vec2 v1 = glm::vec2(1.0, 0.0);
    glm::vec2 v2 = glm::vec2(1.0, 0.0);
    float degrees_90 = glm::pi<float>() / 2.0;
    glm::vec2 output1 = Round::round(Equations::rotate(v1, degrees_90));

    Equations::rotate_mut(v2, degrees_90);
    glm::vec2 output2 = Round::round(v2);

    EXPECT_EQ(output1, output2)
        << "Expected " << io::to_string(output1) << " found " << io::to_string(output2);
}

TEST(EquationsTest, RotateMut3dVecShouldProduceSameResultAsRotate) {
    glm::vec3 v1 = glm::vec3(1.0, 0.0, 0.0);
    glm::vec3 v2 = glm::vec3(1.0, 0.0, 0.0);
    float degrees_90 = glm::pi<float>() / 2.0;
    glm::vec3 output1 = Round::round(Equations::rotate_z(v1, degrees_90));

    Equations::rotate_z_mut(v2, degrees_90);
    glm::vec3 output2 = Round::round(v2);

    EXPECT_EQ(output1, output2)
        << "Expected " << io::to_string(output1) << " found " << io::to_string(output2);
}
