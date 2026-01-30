#pragma once

#include "math/equations.h"
#include <glm/glm.hpp>

namespace math {

constexpr bool is_point_inside_rectangle(const glm::vec2 &point,
                                         const glm::vec2 &rect_center,
                                         const float rect_width, const float rect_height,
                                         const float rect_rotation = 0.0f) {
    glm::vec2 local = point - rect_center;
    rotate_mut(local, -rect_rotation);

    const float half_width = rect_width / 2.0;
    const float half_height = rect_height / 2.0;

    return std::abs(local.x) <= half_width && std::abs(local.y) <= half_height;
};
} // namespace math
