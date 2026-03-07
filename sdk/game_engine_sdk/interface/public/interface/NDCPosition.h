#pragma once
#include "interface/traits.h"
#include "math/Vector2.h"

namespace interface {
/**
 * @struct NDCPoint
 * @brief Represents a normalized device coordinate point (2D)
 *
 * This structure inherits from glm::vec2 and is used to represent
 * points in normalized device coordinate space for 2D rendering operations.
 *
 * Coordinate System:
 * - Range: [-1.0, +1.0] on both x and y axes
 * - Center point: (0, 0) at the midpoint of the space
 * - Left/Bottom boundaries: -1.0
 * - Right/Top boundaries: +1.0
 */
struct NDCPoint : public math::Vector2 {

    NDCPoint() : math::Vector2() {}

    /**
     * @brief Constructor from scalar values
     * @tparam T Must be StaticCastableToFloat
     * @param x X-coordinate in NDC space (-1.0 to 1.0)
     * @param y Y-coordinate in NDC space (-1.0 to 1.0)
     */
    template <StaticCastableToFloat T>
    NDCPoint(T x, T y) : math::Vector2(static_cast<float>(x), static_cast<float>(y)) {}

    /**
     * @brief Constructor from another vec2
     * @param v Source vector to copy from
     */
    NDCPoint(const math::Vector2 &v) : math::Vector2(v) {}

    NDCPoint(NDCPoint &&other) noexcept = default;
    NDCPoint(const NDCPoint &other) = default;
    NDCPoint &operator=(NDCPoint &&other) noexcept = default;
    NDCPoint &operator=(const NDCPoint &other) = default;

    /**
     * @brief Stream output operator for debugging and logging
     * @param os Output stream
     * @param vp The point to format
     * @return Reference to the output stream
     */
    friend std::ostream &operator<<(std::ostream &os, const NDCPoint &vp) {
        os << "NDCPoint(" << vp.x() << ", " << vp.y() << ")";
        return os;
    }

    NDCPoint operator-(const NDCPoint &other) const { return subtract(other); }
    NDCPoint operator+(const NDCPoint &other) const { return add(other); }
    NDCPoint operator*(const NDCPoint &other) const { return multiply(other); }
};
} // namespace interface
