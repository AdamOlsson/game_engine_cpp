#include "equations/projection.h"
#include "equations/round.h"
#include "glm/ext/scalar_constants.hpp"
#include "physics_engine/RigidBody.h"
#include <gtest/gtest.h>

TEST(ProjectionTest, GivenTriangleAtOrigoCreatesExpectedProjection) {
    ShapeData test_body_shape =
        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 1.0}}};
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = 0.0,
                                    .shape_data = test_body_shape};
    glm::vec3 project_axis = glm::vec3(1.0, 0.0, 0.0);
    Projection output = Projection::project_body_on_axis(test_body, project_axis);
    Round::round_mut(output);

    Projection expected = Projection{
        .min = -0.5,
        .max = 0.5,
    };

    EXPECT_EQ(expected, output)
        << "Expected " << expected << " but found " << output << std::endl;
}

TEST(ProjectionTest, GivenTriangleIsOffsetCreatesExpectedProjection) {
    ShapeData test_body_shape =
        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 1.0}}};
    RigidBody test_body = RigidBody{.position = WorldPoint(10.0, 20.0, 0.0),
                                    .rotation = 0.0,
                                    .shape_data = test_body_shape};
    glm::vec3 project_axis = glm::vec3(0.0, 1.0, 0.0);
    Projection output = Projection::project_body_on_axis(test_body, project_axis);
    Round::round_mut(output);

    Projection expected = Projection{
        .min = 19.711,
        .max = 20.577,
    };

    EXPECT_EQ(expected, output)
        << "Expected " << expected << " but found " << output << std::endl;
}

TEST(ProjectionTest, GivenTriangleAtOrigoAndRotatedCreatesExpectedProjection) {
    ShapeData test_body_shape =
        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 1.0}}};
    float degrees_30 = glm::pi<float>() / 6.0;
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, 0.0, 0.0),
                                    .rotation = degrees_30,
                                    .shape_data = test_body_shape};
    glm::vec3 project_axis = glm::vec3(0.0, -1.0, 0.0);
    Projection output = Projection::project_body_on_axis(test_body, project_axis);
    Round::round_mut(output);

    Projection expected = Projection{
        .min = -0.5,
        .max = 0.5,
    };

    EXPECT_EQ(expected, output)
        << "Expected " << expected << " but found " << output << std::endl;
}

TEST(ProjectionTest, GivenTriangleIsOffsetAndRotatedCreatesExpectedProjection) {
    ShapeData test_body_shape =
        ShapeData{.shape = Shape::Triangle, .param = {.triangle = {.side = 1.0}}};
    float degrees_30 = glm::pi<float>() / 6.0;
    RigidBody test_body = RigidBody{.position = WorldPoint(0.0, -0.5, 0.0),
                                    .rotation = degrees_30,
                                    .shape_data = test_body_shape};
    glm::vec3 project_axis = glm::vec3(0.0, -1.0, 0.0);
    Projection output = Projection::project_body_on_axis(test_body, project_axis);
    Round::round_mut(output);

    Projection expected = Projection{
        .min = 0.0,
        .max = 1.0,
    };

    EXPECT_EQ(expected, output)
        << "Expected " << expected << " but found " << output << std::endl;
}

TEST(ProjectionTest, GivenProjectionsDoNotOverlap1) {
    Projection proj1 = Projection{
        .min = 0.0,
        .max = 1.0,
    };

    Projection proj2 = Projection{
        .min = 1.01,
        .max = 2.0,
    };
    Overlap o = proj1.overlap(proj2);
    float expected = -0.01;
    Round::round_mut(o);
    EXPECT_EQ(expected, o.distance);
}

TEST(ProjectionTest, GivenProjectionsDoNotOverlap2) {
    Projection proj1 = Projection{
        .min = 0.0,
        .max = 1.0,
    };

    Projection proj2 = Projection{
        .min = 1.01,
        .max = 2.0,
    };
    Overlap o = proj2.overlap(proj1);
    float expected = -0.01;
    Round::round_mut(o);
    EXPECT_EQ(expected, o.distance);
}

TEST(ProjectionTest, GivenProjectionsDoOverlap) {
    Projection proj1 = Projection{
        .min = 10.0,
        .max = 20.0,
    };

    Projection proj2 = Projection{
        .min = 15.0,
        .max = 21.0,
    };
    Overlap o = proj2.overlap(proj1);
    float expected = 5.0;
    Round::round_mut(o);
    EXPECT_EQ(expected, o.distance);
}

TEST(ProjectionTest, GivenProjectionsAreContained) {
    Projection proj1 = Projection{
        .min = 10.0,
        .max = 20.0,
    };

    Projection proj2 = Projection{
        .min = 11.0,
        .max = 19.0,
    };
    Overlap o = proj2.overlap(proj1);
    float expected = 8.0;
    Round::round_mut(o);
    EXPECT_EQ(expected, o.distance);
}
