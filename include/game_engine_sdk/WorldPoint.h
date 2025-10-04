#pragma once
#include "glm/glm.hpp"
#include <ostream>

struct WorldPoint : public glm::vec3 {
    using glm::vec3::vec3;

    WorldPoint(const glm::vec3 &vec) : glm::vec3(vec) {}
    WorldPoint(const glm::vec2 &vec) : glm::vec3(vec.x, vec.y, 0.0f) {}

    WorldPoint(float x, float y, float z) : glm::vec3(x, y, z) {}
    WorldPoint(float x, float y) : glm::vec3(x, y, 0.0f) {}

    template <typename T>
    WorldPoint(T x, T y)
        : glm::vec3(static_cast<float>(x), static_cast<float>(y), 0.0f) {}

    template <typename T>
    WorldPoint(T x, T y, T z)
        : glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)) {
    }

    friend std::ostream &operator<<(std::ostream &os, const WorldPoint &wp) {
        os << "WorldPoint(" << wp.x << ", " << wp.y << ", " << wp.z << ")";
        return os;
    }
};
