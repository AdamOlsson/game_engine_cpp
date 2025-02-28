#include "physics_engine/collision_resolver.h"
#include "equations/equations.h"
#include "io.h"
#include <optional>

std::ostream &operator<<(std::ostream &os, const Correction &c) {
    return os << "Correction( position: " << c.position << ",  velocity: " << c.velocity
              << ", angular_vel: " << c.angular_velocity << ")";
}

std::ostream &operator<<(std::ostream &os, const CollisionCorrections &c) {
    return os << "CollisionCorrections( body_a: " << c.body_a << ",  body_b: " << c.body_b
              << ")";
}

std::optional<CollisionCorrections>
resolve_collision(const CollisionInformation &ci, RigidBody &body_a, RigidBody &body_b) {

    glm::vec3 p = ci.contact_patch[ci.deepest_contact_idx];
    glm::vec3 collision_normal = ci.normal;
    float penetration_depth = ci.penetration_depth;

    glm::vec3 body_a_center_to_p = p - body_a.position;
    glm::vec3 body_a_center_to_p_perp = Equations::clockwise_perp_z(body_a_center_to_p);
    glm::vec3 body_a_vel_at_p =
        body_a.velocity + body_a.angular_velocity * body_a_center_to_p_perp;

    glm::vec3 body_b_center_to_p = p - body_b.position;
    glm::vec3 body_b_center_to_p_perp = Equations::clockwise_perp_z(body_b_center_to_p);
    glm::vec3 body_b_vel_at_p =
        body_b.velocity + body_b.angular_velocity * body_b_center_to_p_perp;

    glm::vec3 relative_vel_at_p = body_a_vel_at_p - body_b_vel_at_p;

    // TODO: This does not seem to work properly
    // If objects are moving away from each other we do not consider it a collision
    /*if (glm::dot(relative_vel_at_p, collision_normal) > 0.0) {*/
    /*    return std::nullopt;*/
    /*}*/

    float c_r = fmin(body_a.collision_restitution, body_b.collision_restitution);

    // Impulse magnitude calculation
    // NOTE: the cross_2d function differs in direction in this project compared to the
    // rust project as we use a left-handed system
    float nominator = -(1.0 + c_r) * glm::dot(relative_vel_at_p, collision_normal);
    float denom_term1 =
        glm::dot(collision_normal, collision_normal) * (1.0f / body_a.mass) +
        (1.0f / body_b.mass);
    float denom_term2 =
        pow(Equations::cross_2d(body_a_center_to_p, collision_normal), 2.0f) /
        body_a.inertia;
    float denom_term3 =
        pow(Equations::cross_2d(body_b_center_to_p, collision_normal), 2.0f) /
        body_b.inertia;
    float impulse_magnitude = nominator / (denom_term1 + denom_term2 + denom_term3);

    // Post collision (linear) velocities calculation
    glm::vec3 body_a_vel_correction =
        (impulse_magnitude / body_a.mass) * collision_normal;
    glm::vec3 body_b_vel_correction =
        (-impulse_magnitude / body_b.mass) * collision_normal;

    // Post collision (angular) velocities calculation
    glm::vec3 impulse = impulse_magnitude * collision_normal;
    float body_a_angular_vel_correction =
        glm::dot(body_a_center_to_p_perp, impulse) / body_a.inertia;
    float body_b_angular_vel_correction =
        glm::dot(body_b_center_to_p_perp, -impulse) / body_b.inertia;

    // Positional corrections
    glm::vec3 body_a_pos_correction = (penetration_depth / (body_a.mass + body_b.mass)) *
                                      body_b.mass * -collision_normal;
    glm::vec3 body_b_pos_correction = (penetration_depth / (body_a.mass + body_b.mass)) *
                                      body_a.mass * collision_normal;

    CollisionCorrections corrections = {
        .body_a =
            {
                .position = body_a_pos_correction,
                .velocity = body_a_vel_correction,
                .angular_velocity = body_a_angular_vel_correction,
            },
        .body_b =
            {
                .position = body_b_pos_correction,
                .velocity = body_b_vel_correction,
                .angular_velocity = body_b_angular_vel_correction,
            },
    };
    return corrections;
}
