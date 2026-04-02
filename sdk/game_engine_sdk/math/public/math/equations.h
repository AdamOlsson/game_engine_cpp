#pragma once

#include "math/traits.h"
#include <cmath>
#include <glm/glm.hpp>

#include <concepts>

namespace math {

constexpr glm::vec2 rotate(const glm::vec2 &point, const float angle) {
    glm::mat2 rotation_matrix = {glm::vec3(cos(angle), -sin(angle), 0.0f),
                                 glm::vec3(sin(angle), cos(angle), 0.0f)};
    return rotation_matrix * point;
}

constexpr void rotate_mut(glm::vec2 &point, const float angle) {
    glm::mat2 rotation_matrix = {glm::vec3(cos(angle), -sin(angle), 0.0f),
                                 glm::vec3(sin(angle), cos(angle), 0.0f)};
    point = rotation_matrix * point;
}

template <Vec2Or3 T> constexpr float length(const T &v) {
    if constexpr (std::same_as<T, glm::vec2>) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    } else {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }
}

constexpr float sqrt(const float f) { return std::sqrt(f); }

template <Vec2Or3 T> constexpr float distance(const T &from, const T &to) {
    const T diff = to - from;
    return length(diff);
}

template <Vec2Or3 T> constexpr float length2(const T &v) {
    if constexpr (std::same_as<T, glm::vec2>) {
        return v.x * v.x + v.y * v.y;
    } else {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }
}

template <Vec2Or3 T> constexpr float distance2(const T &from, const T &to) {
    const T diff = to - from;
    return length2(diff);
}

constexpr float angle_to_x_axis(const glm::vec2 &vec) { return std::atan2(vec.y, vec.x); }

} // namespace math
