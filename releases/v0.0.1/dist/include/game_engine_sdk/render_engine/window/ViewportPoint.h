#pragma once
#include "game_engine_sdk/WorldPoint.h"
#include <glm/glm.hpp>
#include <ostream>
namespace window {

struct ViewportPoint : public glm::vec2 {
    using glm::vec2::vec2;

    ViewportPoint(float x, float y) : glm::vec2(x, y) {}
    ViewportPoint(double x, double y)
        : glm::vec2(static_cast<float>(x), static_cast<float>(y)) {}

    WorldPoint to_world_point(const glm::vec2 &camera_center) {
        return WorldPoint(this->x - camera_center.x, -(this->y - camera_center.y), 0.0);
    }

    friend std::ostream &operator<<(std::ostream &os, const ViewportPoint &vp) {
        os << "ViewportPoint(" << vp.x << ", " << vp.y << ")";
        return os;
    }
};

} // namespace window
