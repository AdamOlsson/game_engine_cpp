#pragma once
#include "glm/glm.hpp"
struct WorldPoint;

struct WorldPoint : public glm::vec3 {
    using glm::vec3::vec3;

    explicit WorldPoint(const glm::vec3 &vec) : glm::vec3(vec) {}
    WorldPoint(float x, float y, float z) : glm::vec3(x, y, z) {}
    WorldPoint(float x, float y) : glm::vec3(x, y, 0.0f) {}
    WorldPoint(double x, double y, float z)
        : glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) {
    }
};
