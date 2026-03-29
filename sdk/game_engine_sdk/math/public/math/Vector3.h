#pragma once

#include "Vector2.h"
#include "traits.h"
#include <format>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <ostream>

namespace math {

class Vector3 {
  private:
    glm::vec3 m_vec;

  public:
    float x() const { return m_vec.x; }
    float &x() { return m_vec.x; }
    float y() const { return m_vec.y; }
    float &y() { return m_vec.y; }
    float z() const { return m_vec.z; }
    float &z() { return m_vec.z; }

    math::Vector2 xy() const { return math::Vector2(m_vec.x, m_vec.y); }

    template <StaticCastableToFloat T>
    constexpr Vector3(const T x, const T y, const T z) : m_vec(x, y, z) {}

    template <StaticCastableToFloat T>
    constexpr Vector3(const Vector2 &vec, const T z) : m_vec(vec.x(), vec.y(), z) {}

    template <StaticCastableToFloat T>
    constexpr Vector3(const T x, const Vector2 &vec) : m_vec(x, vec.x(), vec.y()) {}

    constexpr Vector3() : m_vec(0.0f, 0.0f, 0.0f) {}

    template <StaticCastableToFloat T> constexpr Vector3(const T v) : m_vec(v, v, v) {}

    constexpr Vector3(glm::vec3 &&vec) : m_vec(vec) {}
    constexpr Vector3(const glm::vec3 &vec) : m_vec(vec) {}

    Vector3(Vector3 &&other) noexcept = default;
    Vector3 &operator=(Vector3 &&other) noexcept = default;

    Vector3(const Vector3 &other) = default;
    Vector3 &operator=(const Vector3 &other) = default;

    constexpr ~Vector3() {}

    operator glm::vec3() const { return m_vec; }

    Vector3 operator+(const Vector3 &other) const {
        return Vector3(m_vec.x + other.m_vec.x, m_vec.y + other.m_vec.y,
                       m_vec.z + other.m_vec.z);
    }

    Vector3 operator-(const Vector3 &other) const {
        return Vector3(m_vec.x - other.m_vec.x, m_vec.y - other.m_vec.y,
                       m_vec.z - other.m_vec.z);
    }

    Vector3 &operator+=(const Vector3 &other) {
        m_vec.x += other.m_vec.x;
        m_vec.y += other.m_vec.y;
        m_vec.z += other.m_vec.z;
        return *this;
    }

    Vector3 &operator-=(const Vector3 &other) {
        m_vec.x -= other.m_vec.x;
        m_vec.y -= other.m_vec.y;
        m_vec.z -= other.m_vec.z;
        return *this;
    }

    Vector3 operator-() const { return Vector3(-m_vec.x, -m_vec.y, -m_vec.z); }

    template <typename Derived> Derived add(const Derived &other) const {
        return static_cast<Derived>(static_cast<const glm::vec3 &>(*this) +
                                    static_cast<const glm::vec3 &>(other));
    }

    template <typename Derived> Derived subtract(const Derived &other) const {
        return static_cast<Derived>(static_cast<const glm::vec3 &>(*this) -
                                    static_cast<const glm::vec3 &>(other));
    }

    template <typename Derived> Derived multiply(const Derived &other) const {
        return static_cast<Derived>(static_cast<const glm::vec3 &>(*this) *
                                    static_cast<const glm::vec3 &>(other));
    }

    friend Vector3 operator+(const glm::vec3 &lhs, const Vector3 &rhs) {
        return Vector3(lhs.x + rhs.x(), lhs.y + rhs.y(), lhs.z + rhs.z());
    }

    friend Vector3 operator+(const Vector3 &lhs, const glm::vec3 &rhs) {
        return Vector3(lhs.x() + rhs.x, lhs.y() + rhs.y, lhs.z() + rhs.z);
    }

    friend Vector3 operator-(const glm::vec3 &lhs, const Vector3 &rhs) {
        return Vector3(lhs.x - rhs.x(), lhs.y - rhs.y(), lhs.z - rhs.z());
    }

    friend Vector3 operator-(const Vector3 &lhs, const glm::vec3 &rhs) {
        return Vector3(lhs.x() - rhs.x, lhs.y() - rhs.y, lhs.z() - rhs.z);
    }

    friend Vector3 operator*(const Vector3 &lhs, const float scalar) {
        return Vector3(lhs.x() * scalar, lhs.y() * scalar, lhs.z() * scalar);
    }

    friend Vector3 operator*(const float scalar, const Vector3 &rhs) {
        return Vector3(scalar * rhs.x(), scalar * rhs.y(), scalar * rhs.z());
    }

    friend Vector3 operator/(const Vector3 &lhs, const float scalar) {
        return Vector3(lhs.x() / scalar, lhs.y() / scalar, lhs.z() / scalar);
    }

    template <StaticCastableToFloat T> Vector3 rotate_x(const T degrees) {
        const glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(degrees),
                                                glm::vec3(1.0f, 0.0f, 0.0f));
        const glm::vec3 rotated_3d = glm::vec3(transform * glm::vec4(m_vec, 1.0f));
        m_vec.x = rotated_3d.x;
        m_vec.y = rotated_3d.y;
        m_vec.z = rotated_3d.z;
        return *this;
    }

    template <StaticCastableToFloat T> Vector3 rotate_y(const T degrees) {
        const glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(degrees),
                                                glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::vec3 rotated_3d = glm::vec3(transform * glm::vec4(m_vec, 1.0f));
        m_vec.x = rotated_3d.x;
        m_vec.y = rotated_3d.y;
        m_vec.z = rotated_3d.z;
        return *this;
    }

    template <StaticCastableToFloat T> Vector3 rotate_z(const T degrees) {
        const glm::mat4 transform = glm::rotate(glm::mat4(1.0f), glm::radians(degrees),
                                                glm::vec3(0.0f, 0.0f, 1.0f));
        const glm::vec3 rotated_3d = glm::vec3(transform * glm::vec4(m_vec, 1.0f));
        m_vec.x = rotated_3d.x;
        m_vec.y = rotated_3d.y;
        m_vec.z = rotated_3d.z;
        return *this;
    }

    std::string to_string() const {
        return "Vector3(" + std::to_string(m_vec.x) + ", " + std::to_string(m_vec.y) +
               ", " + std::to_string(m_vec.z) + ")";
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector3 &v) {
        os << v.to_string();
        return os;
    }
};

} // namespace math

template <> struct std::formatter<math::Vector3> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }
    auto format(const math::Vector3 &v, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "Vector3({}, {}, {})", v.x(), v.y(), v.z());
    }
};
