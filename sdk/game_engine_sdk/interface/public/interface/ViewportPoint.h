#pragma once
#include "interface/traits.h"
#include "math/Vector2.h"

namespace interface {
/**
 * @struct ViewportPoint
 * @brief Represents a pixel position relative to the center of the viewport/window.
 *
 * Unlike standard screen coordinates which often start at (0,0) in the top-left,
 * this structure uses a centered coordinate system suitable for offset calculations
 * and symmetric UI layouts.
 *
 * @tparam Inherited Type glm::vec2 provides vector arithmetic functionality
 */
struct ViewportPoint : public math::Vector2 {

    ViewportPoint() : math::Vector2() {}

    /**
     * @brief Constructor from scalar values
     * @tparam T Value type convertible to float (StaticCastableToFloat)
     * @param x X-offset in pixels relative to the viewport center (-width/2 to width/2)
     * @param y Y-offset in pixels relative to the viewport center (-height/2 to height/2)
     */
    template <StaticCastableToFloat T>
    ViewportPoint(T x, T y)
        : math::Vector2(static_cast<float>(x), static_cast<float>(y)) {}

    /**
     * @brief Constructor from a vector
     * @param v Source vector containing the x and y offsets
     */
    ViewportPoint(const math::Vector2 &v) : math::Vector2(v) {}

    ViewportPoint(ViewportPoint &&other) noexcept = default;
    ViewportPoint(const ViewportPoint &other) = default;
    ViewportPoint &operator=(ViewportPoint &&other) noexcept = default;
    ViewportPoint &operator=(const ViewportPoint &other) = default;

    /**
     * @brief Stream output operator for debugging and logging
     * @param os Output stream
     * @param vp The point to format
     * @return Reference to the output stream
     */
    friend std::ostream &operator<<(std::ostream &os, const ViewportPoint &vp) {
        os << "ViewportPoint(" << vp.x() << ", " << vp.y() << ")";
        return os;
    }

    ViewportPoint operator-(const ViewportPoint &other) const { return subtract(other); }
    ViewportPoint operator+(const ViewportPoint &other) const { return add(other); }
    ViewportPoint operator*(const ViewportPoint &other) const { return multiply(other); }
};
} // namespace interface
