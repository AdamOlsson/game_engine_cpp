#include "camera/Camera.h"
#include "logger/logger.h"
#include "util/io.h"
#include <gtest/gtest.h>

TEST(Camera2DTest, Test_SetRelativeCameraPosition) {
    const auto window_width = 100.0f;
    const auto window_height = 100.0f;
    const auto inital_zoom = 1.0f;
    auto camera = camera::Camera2D(window_width, window_height, inital_zoom);

    const auto zoom_center = camera::ViewportPoint(0, 0);
    const auto world_space_pos_delta = camera::WorldPoint2D(0, 0);
    camera.set_relative_position(world_space_pos_delta);
    camera.set_relative_zoom(0.0f, zoom_center);
    camera.set_relative_rotation(0.0f);

    const auto view_mat = camera.get_view_matrix();
    // clang-format off
    const auto expected_view_mat = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on

    EXPECT_EQ(expected_view_mat, view_mat)
        << std::format("Expected {} but found {}", io::to_string(expected_view_mat),
                       io::to_string(view_mat));

    const auto world_space_pos = camera::WorldPoint2D(0, 0);
    const auto proj_mat = camera.get_projection_matrix();
    const glm::vec4 projected_pos = glm::vec4(world_space_pos, 0.0f, 1.0f) * proj_mat;
    const auto expected_projected_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(expected_projected_pos, projected_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_projected_pos),
                       io::to_string(projected_pos));
}

TEST(Camera2DTest, Test_SetAbsoluteCameraPosition) {
    const auto window_width = 100.0f;
    const auto window_height = 100.0f;
    const auto inital_zoom = 1.0f;
    auto camera = camera::Camera2D(window_width, window_height, inital_zoom);

    const auto zoom_center = camera::ViewportPoint(0, 0);
    const auto world_space_pos = camera::WorldPoint2D(0, 0);
    camera.set_position(world_space_pos);
    camera.set_zoom(1.0f, zoom_center);
    camera.set_rotation(0.0f);

    const auto view_mat = camera.get_view_matrix();
    // clang-format off
    const auto expected_view_mat = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on

    EXPECT_EQ(expected_view_mat, view_mat)
        << std::format("Expected {} but found {}", io::to_string(expected_view_mat),
                       io::to_string(view_mat));

    const auto proj_mat = camera.get_projection_matrix();
    const glm::vec4 projected_pos = glm::vec4(world_space_pos, 0.0f, 1.0f) * proj_mat;
    const auto expected_projected_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(expected_projected_pos, projected_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_projected_pos),
                       io::to_string(projected_pos));
}
