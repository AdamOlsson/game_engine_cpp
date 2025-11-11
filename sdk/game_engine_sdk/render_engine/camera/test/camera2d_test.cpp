#include "camera/Camera.h"
#include "util/io.h"
#include <gtest/gtest.h>

TEST(Camera2DTest, Test_SetRelativeCameraZoomOffOrigo) {
    const auto window_width = 100.0f;
    const auto window_height = 100.0f;
    const auto base_orthogonal_height = 100.0f;
    auto camera = camera::Camera2D(window_width, window_height, base_orthogonal_height);

    camera.set_position(camera::WorldPoint2D(100, 100));
    camera.set_rotation(0.0f);
    camera.set_relative_zoom(3.0f);

    const auto view_mat = camera.get_view_matrix();

    // NOTE TO FUTURE ADAM: This expected view matrix is not necessarily true as I only
    // took the actual output and set it as expected.
    // clang-format off
    const auto expected_view_mat = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            -100.0f, -100.0f, 0.0f, 1.0f
    );
    // clang-format on
    EXPECT_EQ(expected_view_mat, view_mat)
        << std::format("Expected {} but found {}", io::to_string(expected_view_mat),
                       io::to_string(view_mat));
    const auto world_space_pos = camera::WorldPoint2D(100, 100);
    const auto view_space_pos = view_mat * glm::vec4(world_space_pos, 0.0f, 1.0f);

    // NOTE TO FUTURE ADAM: This expected view space position is not necessarily true as I
    // only took the actual output and set it as expected.
    const auto expected_view_space_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(expected_view_space_pos, view_space_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_view_space_pos),
                       io::to_string(view_space_pos));

    const auto proj_mat = camera.get_projection_matrix();
    // clang-format off
    const auto expected_proj_mat = glm::mat4(
            0.080f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.080f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on

    EXPECT_EQ(expected_proj_mat, proj_mat)
        << std::format("Expected {} but found {}", io::to_string(expected_proj_mat),
                       io::to_string(proj_mat));

    const glm::vec4 clip_space_pos = view_space_pos * proj_mat;
    // NOTE TO FUTURE ADAM: This expected clip position is not necessarily true as I only
    // took the actual output and set it as expected.
    const auto expected_clip_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(expected_clip_pos, clip_space_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_clip_pos),
                       io::to_string(clip_space_pos));
}

TEST(Camera2DTest, Test_SetRelativeCameraZoomAtOrigo) {
    const auto window_width = 100.0f;
    const auto window_height = 100.0f;
    const auto base_orthogonal_height = 100.0f;
    auto camera = camera::Camera2D(window_width, window_height, base_orthogonal_height);

    const auto zoom_center = camera::ViewportPoint(0, 0);
    camera.set_position(camera::WorldPoint2D(0, 0));
    camera.set_relative_zoom(3.0f);
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
    const auto world_space_pos = camera::WorldPoint2D(0, 0);
    const auto view_space_pos = view_mat * glm::vec4(world_space_pos, 0.0f, 1.0f);

    const auto expected_view_space_pos = glm::vec4(0, 0, 0, 1);
    EXPECT_EQ(expected_view_space_pos, view_space_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_view_space_pos),
                       io::to_string(view_space_pos));

    const auto proj_mat = camera.get_projection_matrix();
    // clang-format off
    const auto expected_proj_mat = glm::mat4(
            0.080f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.080f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on

    EXPECT_EQ(expected_proj_mat, proj_mat)
        << std::format("Expected {} but found {}", io::to_string(expected_proj_mat),
                       io::to_string(proj_mat));

    const glm::vec4 clip_space_pos = view_space_pos * proj_mat;
    const auto expected_clip_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(expected_clip_pos, clip_space_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_clip_pos),
                       io::to_string(clip_space_pos));
}

TEST(Camera2DTest, Test_SetRelativeCameraPositionAtOrigoAndWorldPointOffOrigo) {
    const auto window_width = 100.0f;
    const auto window_height = 100.0f;
    const auto base_orthogonal_height = 100.0f;
    auto camera = camera::Camera2D(window_width, window_height, base_orthogonal_height);

    camera.set_relative_position(camera::WorldPoint2D(0, 0));
    camera.set_relative_zoom(0.0f);
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
    const auto world_space_pos = camera::WorldPoint2D(25, 25);
    const auto view_space_pos = view_mat * glm::vec4(world_space_pos, 0.0f, 1.0f);

    const auto expected_view_space_pos = glm::vec4(25, 25, 0, 1);
    EXPECT_EQ(expected_view_space_pos, view_space_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_view_space_pos),
                       io::to_string(view_space_pos));

    const auto proj_mat = camera.get_projection_matrix();
    // clang-format off
    const auto expected_proj_mat = glm::mat4(
            0.020f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.020f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on

    EXPECT_EQ(expected_proj_mat, proj_mat)
        << std::format("Expected {} but found {}", io::to_string(expected_proj_mat),
                       io::to_string(proj_mat));

    const glm::vec4 clip_space_pos = view_space_pos * proj_mat;
    const auto expected_clip_pos = glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
    EXPECT_EQ(expected_clip_pos, clip_space_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_clip_pos),
                       io::to_string(clip_space_pos));
}

TEST(Camera2DTest, Test_SetRelativeCameraPositionAtOrigo) {
    const auto window_width = 100.0f;
    const auto window_height = 100.0f;
    const auto base_orthogonal_height = 100.0f;
    auto camera = camera::Camera2D(window_width, window_height, base_orthogonal_height);

    camera.set_relative_position(camera::WorldPoint2D(0, 0));
    camera.set_zoom(1.0f);
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

    const auto world_space_pos = camera::WorldPoint2D(0, 0);
    const auto view_space_pos = view_mat * glm::vec4(world_space_pos, 0.0f, 1.0f);

    const auto expected_view_space_pos = glm::vec4(0, 0, 0, 1);
    EXPECT_EQ(expected_view_space_pos, view_space_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_view_space_pos),
                       io::to_string(view_space_pos));

    const auto proj_mat = camera.get_projection_matrix();
    // clang-format off
    const auto expected_proj_mat = glm::mat4(
            0.020f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.020f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on

    EXPECT_EQ(expected_proj_mat, proj_mat)
        << std::format("Expected {} but found {}", io::to_string(expected_proj_mat),
                       io::to_string(proj_mat));

    const glm::vec4 clip_space_pos = proj_mat * view_space_pos;
    const auto expected_clip_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(expected_clip_pos, clip_space_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_clip_pos),
                       io::to_string(clip_space_pos));
}

TEST(Camera2DTest, Test_SetRelativeCameraPositionOffOrigoAndWorldPointOnOrigo) {
    const auto window_width = 100.0f;
    const auto window_height = 100.0f;
    const auto base_orthogonal_height = 100.0f;
    auto camera = camera::Camera2D(window_width, window_height, base_orthogonal_height);

    const auto zoom_center = camera::ViewportPoint(0, 0);
    camera.set_relative_position(camera::WorldPoint2D(25, 25));
    camera.set_zoom(1.0f);
    camera.set_rotation(0.0f);

    const auto view_mat = camera.get_view_matrix();
    // clang-format off
    const auto expected_view_mat = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            -25.0f, -25.0f, 0.0f, 1.0f
    );
    // clang-format on

    EXPECT_EQ(expected_view_mat, view_mat)
        << std::format("Expected {} but found {}", io::to_string(expected_view_mat),
                       io::to_string(view_mat));

    const auto world_space_pos = camera::WorldPoint2D(0, 0);
    const auto view_space_pos = view_mat * glm::vec4(world_space_pos, 0.0f, 1.0f);

    const auto expected_view_space_pos = glm::vec4(-25, -25, 0, 1);
    EXPECT_EQ(expected_view_space_pos, view_space_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_view_space_pos),
                       io::to_string(view_space_pos));

    const auto proj_mat = camera.get_projection_matrix();
    // clang-format off
    const auto expected_proj_mat = glm::mat4(
            0.020f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.020f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on

    EXPECT_EQ(expected_proj_mat, proj_mat)
        << std::format("Expected {} but found {}", io::to_string(expected_proj_mat),
                       io::to_string(proj_mat));

    const glm::vec4 clip_space_pos = proj_mat * view_space_pos;
    const auto expected_clip_pos = glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    EXPECT_EQ(expected_clip_pos, clip_space_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_clip_pos),
                       io::to_string(clip_space_pos));
}

TEST(Camera2DTest, Test_SetAbsoluteCameraPosition) {
    const auto window_width = 100.0f;
    const auto window_height = 100.0f;
    const auto base_orthogonal_height = 100.0f;
    auto camera = camera::Camera2D(window_width, window_height, base_orthogonal_height);

    camera.set_position(camera::WorldPoint2D(0, 0));
    camera.set_zoom(1.0f);
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

    const auto world_space_pos = camera::WorldPoint2D(0, 0);
    const auto view_space_pos = view_mat * glm::vec4(world_space_pos, 0.0f, 1.0f);

    const auto expected_view_space_pos = glm::vec4(0, 0, 0, 1);
    EXPECT_EQ(expected_view_space_pos, view_space_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_view_space_pos),
                       io::to_string(view_space_pos));

    const auto proj_mat = camera.get_projection_matrix();
    // clang-format off
    const auto expected_proj_mat = glm::mat4(
            0.020f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.020f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
    );
    // clang-format on

    EXPECT_EQ(expected_proj_mat, proj_mat)
        << std::format("Expected {} but found {}", io::to_string(expected_proj_mat),
                       io::to_string(proj_mat));

    const glm::vec4 projected_pos = proj_mat * view_space_pos;
    const auto expected_projected_pos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    EXPECT_EQ(expected_projected_pos, projected_pos)
        << std::format("Expected {} but found {}", io::to_string(expected_projected_pos),
                       io::to_string(projected_pos));
}
