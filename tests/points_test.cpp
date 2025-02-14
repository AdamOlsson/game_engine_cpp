#include "render_engine/Window.h"
#include <gtest/gtest.h>

TEST(PointsTest, ViewportPointAtTopLeftToWorldPointTest) {
    glm::vec2 camera_center = glm::vec2(400.0f, 400.0f);
    ViewportPoint vp = ViewportPoint(0.0f, 0.0f);
    WorldPoint wp = vp.to_world_point(camera_center);
    EXPECT_EQ(wp.x, -400.0f);
    EXPECT_EQ(wp.y, 400.0f);
    EXPECT_EQ(wp.z, 0.0f);
}

TEST(PointsTest, ViewportPointAtCenterToWorldPointTest) {
    glm::vec2 camera_center = glm::vec2(400.0f, 400.0f);
    ViewportPoint vp = ViewportPoint(400.0f, 400.0f);
    WorldPoint wp = vp.to_world_point(camera_center);
    EXPECT_EQ(wp.x, 0.0f);
    EXPECT_EQ(wp.y, 0.0f);
    EXPECT_EQ(wp.z, 0.0f);
}

TEST(PointsTest, ViewportPointAtBottomRightToWorldPointTest) {
    glm::vec2 camera_center = glm::vec2(400.0f, 400.0f);
    ViewportPoint vp = ViewportPoint(800.0f, 800.0f);
    WorldPoint wp = vp.to_world_point(camera_center);
    EXPECT_EQ(wp.x, 400.0f);
    EXPECT_EQ(wp.y, -400.0f);
    EXPECT_EQ(wp.z, 0.0f);
}
