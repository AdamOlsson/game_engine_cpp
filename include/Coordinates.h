#pragma once
#include "glm/glm.hpp"
struct ViewportPoint;
struct WorldPoint;

struct WorldPoint : public glm::vec3 {
    using glm::vec3::vec3;

    explicit WorldPoint(const glm::vec3 &vec) : glm::vec3(vec) {}
    WorldPoint(float x, float y, float z) : glm::vec3(x, y, z) {}
    WorldPoint(float x, float y) : glm::vec3(x, y, 0.0f) {}
    WorldPoint(double x, double y, float z)
        : glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) {
    }

    /*ViewportPoint to_viewport_point(const uint32_t window_width,*/
    /*                                const uint32_t window_height);*/
};

struct ViewportPoint : public glm::vec2 {
    using glm::vec2::vec2;

    ViewportPoint(float x, float y) : glm::vec2(x, y) {}
    ViewportPoint(double x, double y)
        : glm::vec2(static_cast<float>(x), static_cast<float>(y)) {}

    WorldPoint to_world_point(const glm::vec2 &camera_center);
};
