#pragma once

#include "traits.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace math {

class Vector2 {
  private:
    glm::vec2 m_vec;

  public:
    float &x;
    float &y;

    template <StaticCastableToFloat T>
    constexpr Vector2(const T x, const T y) : m_vec(x, y), x(m_vec.x), y(m_vec.y) {}

    template <StaticCastableToFloat T> Vector2 rotate_z(const T degrees) {
        const glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(degrees),
                                                glm::vec3(0.0f, 0.0f, 1.0f));
        const glm::vec3 rotated_3d = glm::vec3(transform * glm::vec4(m_vec, 0.0f, 1.0f));
        m_vec.x = rotated_3d.x;
        m_vec.y = rotated_3d.y;
        return *this;
    }

    operator glm::vec2() const { return m_vec; }
};

} // namespace math
