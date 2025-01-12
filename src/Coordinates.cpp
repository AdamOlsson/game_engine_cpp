#include "Coordinates.h"

WorldPoint ViewportPoint::to_world_point(const glm::vec2 &camera_center) {
    return WorldPoint(this->x - camera_center.x, -(this->y - camera_center.y), 0.0);
}
ViewportPoint WorldPoint::to_viewport_point(const uint32_t window_width,
                                            const uint32_t window_height) {}
