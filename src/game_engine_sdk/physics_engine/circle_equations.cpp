#include "circle_equations.h"
#include "game_engine_sdk/equations/equations.h"

float get_circle_bounding_volume_radius(const RigidBody &body) {
    const auto circle = body.shape.get<Circle>();
    return circle.diameter;
}

bool is_point_inside_circle(const RigidBody &body, const WorldPoint &point) {
    const auto circle = body.shape.get<Circle>();
    const auto radius = circle.diameter / 2.0f;
    return Equations::length2(point - body.position) <= radius * radius;
}

/// For a given point, calculate the closest point on the rigid body. If the
/// given point is inside the rigid body, the given point will be returned.
WorldPoint closest_point_on_circle(const RigidBody &body, const WorldPoint &point) {
    const auto circle = body.shape.get<Circle>();
    const glm::vec3 diff = point - body.position;
    const auto radius = circle.diameter / 2.0f;

    if (Equations::length2(diff) <= radius * radius) {
        // Inside body
        return point;
    } else {
        // Outside body
        return static_cast<WorldPoint>(glm::normalize(diff) * radius);
    }
}

float circle_inertia(const RigidBody &body) {
    const auto circle = body.shape.get<Circle>();
    const auto radius = circle.diameter / 2.0f;
    return 0.5f * body.mass * radius * radius;
}
