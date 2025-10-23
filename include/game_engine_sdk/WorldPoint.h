#pragma once
#include "game_engine_sdk/traits.h"
#include "glm/glm.hpp"
#include <ostream>

struct WorldPoint2D : public glm::vec2 {
    using glm::vec2::vec2;

    template <StaticCastableToFloat T>
    WorldPoint2D(T x, T y) : glm::vec2(static_cast<float>(x), static_cast<float>(y)) {}

    WorldPoint2D(const glm::vec2 &v) : glm::vec2(v) {}

    friend std::ostream &operator<<(std::ostream &os, const WorldPoint2D &wp) {
        os << "WorldPoint2D(" << wp.x << ", " << wp.y << ", " << ")";
        return os;
    }
};

// Deprecated
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
