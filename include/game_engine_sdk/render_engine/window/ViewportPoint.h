#pragma once
#include "glm/glm.hpp"
#include <ostream>

namespace window {

template <typename T>
concept StaticCastableToFloat = requires(T t) {
    { static_cast<float>(t) } -> std::same_as<float>;
};

struct ViewportPoint : public glm::vec2 {
    using glm::vec2::vec2;

    template <StaticCastableToFloat T>
    ViewportPoint(T x, T y) : glm::vec2(static_cast<float>(x), static_cast<float>(y)) {}

    ViewportPoint(const glm::vec2 &v) : glm::vec2(v) {}

    friend std::ostream &operator<<(std::ostream &os, const ViewportPoint &vp) {
        os << "ViewportPoint(" << vp.x << ", " << vp.y << ")";
        return os;
    }
};

} // namespace window
