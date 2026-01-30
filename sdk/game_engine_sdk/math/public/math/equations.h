#pragma once

#include <cmath>
#include <glm/glm.hpp>

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

} // namespace math
