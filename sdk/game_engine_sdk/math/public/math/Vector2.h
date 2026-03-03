#pragma once

#include "traits.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <ostream>

namespace math {

class Vector2 {
  private:
    glm::vec2 m_vec;

  public:
    float x() const { return m_vec.x; }
    float &x() { return m_vec.x; }
    float y() const { return m_vec.y; }
    float &y() { return m_vec.y; }

    template <StaticCastableToFloat T>
    constexpr Vector2(const T x, const T y) : m_vec(x, y) {}

    constexpr Vector2() : m_vec(0.0f, 0.0f) {}
    constexpr Vector2(glm::vec2 &&vec) : m_vec(vec) {}
    constexpr Vector2(const glm::vec2 &vec) : m_vec(vec) {}

    Vector2(Vector2 &&other) noexcept = default;
    Vector2 &operator=(Vector2 &&other) noexcept = default;

    Vector2(const Vector2 &other) = default;
    Vector2 &operator=(const Vector2 &other) = default;

    constexpr ~Vector2() {}

    template <StaticCastableToFloat T> Vector2 rotate_z(const T degrees) {
        const glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(degrees),
                                                glm::vec3(0.0f, 0.0f, 1.0f));
        const glm::vec3 rotated_3d = glm::vec3(transform * glm::vec4(m_vec, 0.0f, 1.0f));
        m_vec.x = rotated_3d.x;
        m_vec.y = rotated_3d.y;
        return *this;
    }

    operator glm::vec2() const { return m_vec; }

    Vector2 operator+(const Vector2 &other) const {
        return Vector2(m_vec.x + other.m_vec.x, m_vec.y + other.m_vec.y);
    }

    Vector2 operator-(const Vector2 &other) const {
        return Vector2(m_vec.x - other.m_vec.x, m_vec.y - other.m_vec.y);
    }

    Vector2 &operator+=(const Vector2 &other) {
        m_vec.x += other.m_vec.x;
        m_vec.y += other.m_vec.y;
        return *this;
    }

    Vector2 &operator-=(const Vector2 &other) {
        m_vec.x -= other.m_vec.x;
        m_vec.y -= other.m_vec.y;
        return *this;
    }

    Vector2 operator-() const { return Vector2(-m_vec.x, -m_vec.y); }

    friend Vector2 operator+(const glm::vec2 &lhs, const Vector2 &rhs) {
        return Vector2(lhs.x + rhs.x(), lhs.y + rhs.y());
    }

    friend Vector2 operator+(const Vector2 &lhs, const glm::vec2 &rhs) {
        return Vector2(lhs.x() + rhs.x, lhs.y() + rhs.y);
    }

    friend Vector2 operator*(const Vector2 &lhs, const float scalar) {
        return Vector2(lhs.x() * scalar, lhs.y() * scalar);
    }

    friend Vector2 operator-(const glm::vec2 &lhs, const Vector2 &rhs) {
        return Vector2(lhs.x - rhs.x(), lhs.y - rhs.y());
    }

    friend Vector2 operator-(const Vector2 &lhs, const glm::vec2 &rhs) {
        return Vector2(lhs.x() - rhs.x, lhs.y() - rhs.y);
    }

    friend Vector2 operator/(const Vector2 &lhs, const float scalar) {
        return Vector2(lhs.x() / scalar, lhs.y() / scalar);
    }

    /*std::string to_string() {}*/
    friend std::ostream &operator<<(std::ostream &os, const Vector2 &v) {
        return os << "Vector2(" << v.x() << ", " << v.y() << ")";
    }
};

} // namespace math
