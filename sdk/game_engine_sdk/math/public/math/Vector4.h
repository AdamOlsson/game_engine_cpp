#pragma once

#include "Vector2.h"
#include "traits.h"
#include <glm/glm.hpp>

namespace math {

class Vector4 {
  private:
    glm::vec4 m_vec;

  public:
    float x() const { return m_vec.x; }
    float &x() { return m_vec.x; }
    float y() const { return m_vec.y; }
    float &y() { return m_vec.y; }
    float z() const { return m_vec.z; }
    float &z() { return m_vec.z; }
    float w() const { return m_vec.w; }
    float &w() { return m_vec.w; }

    math::Vector2 xy() const { return math::Vector2(m_vec.x, m_vec.y); }
    math::Vector2 zw() const { return math::Vector2(m_vec.z, m_vec.w); }

    template <StaticCastableToFloat T>
    constexpr Vector4(const T x, const T y, const T z, const T w) : m_vec(x, y, z, w) {}

    constexpr Vector4(const Vector2 &a, const Vector2 &b)
        : m_vec(a.x(), a.y(), b.x(), b.y()) {}

    constexpr Vector4() : m_vec(0.0f, 0.0f, 0.0f, 0.0f) {}

    template <StaticCastableToFloat T> constexpr Vector4(const T v) : m_vec(v, v, v, v) {}

    Vector4(Vector4 &&other) noexcept = default;
    Vector4 &operator=(Vector4 &&other) noexcept = default;

    Vector4(const Vector4 &other) = default;
    Vector4 &operator=(const Vector4 &other) = default;

    constexpr ~Vector4() {}

    operator glm::vec4() const { return m_vec; }

    Vector4 operator+(const Vector4 &other) const {
        return Vector4(m_vec.x + other.m_vec.x, m_vec.y + other.m_vec.y,
                       m_vec.z + other.m_vec.z, m_vec.w + other.m_vec.w);
    }

    Vector4 operator-(const Vector4 &other) const {
        return Vector4(m_vec.x - other.m_vec.x, m_vec.y - other.m_vec.y,
                       m_vec.z - other.m_vec.z, m_vec.w - other.m_vec.w);
    }

    Vector4 &operator+=(const Vector4 &other) {
        m_vec.x += other.m_vec.x;
        m_vec.y += other.m_vec.y;
        m_vec.z += other.m_vec.z;
        m_vec.w += other.m_vec.w;
        return *this;
    }

    Vector4 &operator-=(const Vector4 &other) {
        m_vec.x -= other.m_vec.x;
        m_vec.y -= other.m_vec.y;
        m_vec.z -= other.m_vec.z;
        m_vec.w -= other.m_vec.w;
        return *this;
    }

    Vector4 operator-() const { return Vector4(-m_vec.x, -m_vec.y, -m_vec.z, -m_vec.w); }

    friend Vector4 operator+(const glm::vec4 &lhs, const Vector4 &rhs) {
        return Vector4(lhs.x + rhs.x(), lhs.y + rhs.y(), lhs.z + rhs.z(),
                       lhs.w + rhs.w());
    }

    friend Vector4 operator+(const Vector4 &lhs, const glm::vec4 &rhs) {
        return Vector4(lhs.x() + rhs.x, lhs.y() + rhs.y, lhs.z() + rhs.z,
                       lhs.w() + rhs.w);
    }

    friend Vector4 operator-(const glm::vec4 &lhs, const Vector4 &rhs) {
        return Vector4(lhs.x - rhs.x(), lhs.y - rhs.y(), lhs.z - rhs.z(),
                       lhs.w - rhs.w());
    }

    friend Vector4 operator-(const Vector4 &lhs, const glm::vec4 &rhs) {
        return Vector4(lhs.x() - rhs.x, lhs.y() - rhs.y, lhs.z() - rhs.z,
                       lhs.w() - rhs.w);
    }

    std::string to_string() const {
        return "Vector4(" + std::to_string(m_vec.x) + ", " + std::to_string(m_vec.y) +
               ", " + std::to_string(m_vec.z) + ", " + std::to_string(m_vec.w) + ")";
    }

    friend std::ostream &operator<<(std::ostream &os, const Vector4 &v) {
        os << v.to_string();
        return os;
    }
};

} // namespace math

template <> struct std::formatter<math::Vector4> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }
    auto format(const math::Vector4 &v, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "Vector4({}, {}, {}, {})", v.x(), v.y(), v.z(),
                              v.w());
    }
};
