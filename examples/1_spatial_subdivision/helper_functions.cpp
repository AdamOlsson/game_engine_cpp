#include "helper_functions.h"
#include "io.h"

void apply_physics(const float dt, RigidBody &body) {
    WorldPoint temp_position = body.position;
    body.position = static_cast<WorldPoint>(2.0f * body.position - body.prev_position +
                                            0.5f * body.acceleration * dt * dt);
    body.prev_position = temp_position;
    body.velocity = (body.position - body.prev_position) / dt;
    body.rotation += body.angular_velocity * dt;
}

void apply_correction(const float dt, const Correction &correction, RigidBody &body) {
    body.position += correction.position;
    body.velocity += correction.velocity;
    body.angular_velocity += correction.angular_velocity;
    body.prev_position = WorldPoint(body.position - body.velocity * dt);
}

void constrain_to_window(const float dt, CollisionSolver &solver, RigidBody &body) {
    // TODO: Check which quad the body is in and only check for that quads corner
    std::optional<CollisionInformation> collision_info;

    collision_info = SAT::collision_detection(body, BOTTOM_BORDER);
    if (collision_info.has_value()) {
        std::optional<CollisionCorrections> correction =
            solver.resolve_collision(collision_info.value(), body, BOTTOM_BORDER);

        if (correction.has_value()) {
            apply_correction(dt, correction.value().body_a, body);
            /*apply_correction(dt, -correction.value().body_b, body);*/
        }
        /*return;*/
    }

    collision_info = SAT::collision_detection(body, RIGHT_BORDER);
    if (collision_info.has_value()) {
        std::optional<CollisionCorrections> correction =
            solver.resolve_collision(collision_info.value(), body, RIGHT_BORDER);

        if (correction.has_value()) {
            apply_correction(dt, correction.value().body_a, body);
            /*apply_correction(dt, -correction.value().body_b, body);*/
        }
        /*return;*/
    }

    collision_info = SAT::collision_detection(body, TOP_BORDER);
    if (collision_info.has_value()) {
        std::optional<CollisionCorrections> correction =
            solver.resolve_collision(collision_info.value(), body, TOP_BORDER);

        if (correction.has_value()) {
            apply_correction(dt, correction.value().body_a, body);
            /*apply_correction(dt, -correction.value().body_b, body);*/
        }
        /*return;*/
    }

    collision_info = SAT::collision_detection(body, LEFT_BORDER);
    if (collision_info.has_value()) {
        std::optional<CollisionCorrections> correction =
            solver.resolve_collision(collision_info.value(), body, LEFT_BORDER);

        if (correction.has_value()) {
            apply_correction(dt, correction.value().body_a, body);
            /*apply_correction(dt, -correction.value().body_b, body);*/
        }
        /*return;*/
    }
}

void check_collision_with_spinner(const float dt, CollisionSolver &solver,
                                  const RigidBody &spinner, RigidBody &body) {
    auto collision_info = SAT::collision_detection(spinner, body);
    if (collision_info.has_value()) {
        std::optional<CollisionCorrections> correction =
            solver.resolve_collision(collision_info.value(), spinner, body);

        if (correction.has_value()) {
            apply_correction(dt, correction.value().body_b, body);
        }
    }
}
