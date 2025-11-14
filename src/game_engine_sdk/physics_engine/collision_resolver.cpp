#include "game_engine_sdk/physics_engine/collision_resolver.h"
#include "game_engine_sdk/equations/equations.h"
#include "game_engine_sdk/physics_engine/RigidBody.h"
#include "logger/io.h"
#include <optional>

inline float calculate_impulse_magnitude(const glm::vec3 &collision_normal,
                                         const glm::vec3 &relative_vel_at_p,
                                         const RigidBody &body_a, const RigidBody &body_b,
                                         const glm::vec3 &body_a_center_to_p,
                                         const glm::vec3 &body_b_center_to_p,
                                         const float c_r);

inline glm::vec3 positional_correction(const glm::vec3 &collision_normal,
                                       const float penetration_depth,
                                       const RigidBody &this_body,
                                       const RigidBody &other_body);

inline glm::vec3 velocity_correction(const glm::vec3 &normal,
                                     const float impulse_magnitude,
                                     const RigidBody &body);

inline float angular_velocity_correction(const glm::vec3 &center_to_p_perp,
                                         const glm::vec3 &impulse, const RigidBody &body);

CollisionSolver::CollisionSolver() : baumgarte_factor(1.0) {}
CollisionSolver::CollisionSolver(float baumegarte_factor)
    : baumgarte_factor(baumegarte_factor) {}

std::optional<CollisionCorrections>
CollisionSolver::resolve_collision(const CollisionInformation &ci,
                                   const RigidBody &body_a, const RigidBody &body_b) {
    switch (ci.contact_type) {
    case ContactType::NONE:
        return std::nullopt;
    case ContactType::VERTEX_VERTEX:
    case ContactType::VERTEX_EDGE: {
        return resolve_collision_vertex_vertex(ci, body_a, body_b);
    }
    case ContactType::EDGE_EDGE:
        return resolve_collision_edge_edge(ci, body_a, body_b);
    };
    return std::nullopt;
}

std::optional<CollisionCorrections> CollisionSolver::resolve_collision_edge_edge(
    const CollisionInformation &ci, const RigidBody &body_a, const RigidBody &body_b) {
    const glm::vec3 body_a_pos_correction =
        positional_correction(-ci.normal, ci.penetration_depth, body_a, body_b) *
        baumgarte_factor;
    const glm::vec3 body_b_pos_correction =
        positional_correction(ci.normal, ci.penetration_depth, body_b, body_a) *
        baumgarte_factor;

    const float c_r = fmin(body_a.collision_restitution, body_b.collision_restitution);

    glm::vec3 midpoint(0.0f);
    for (auto &contact_point : ci.contact_patch) {
        midpoint += contact_point;
    }
    midpoint /= static_cast<float>(ci.contact_patch.size());

    const glm::vec3 body_a_center_to_mid = midpoint - body_a.position;
    const glm::vec3 body_a_center_to_mid_perp =
        Equations::clockwise_perp_z(body_a_center_to_mid);
    const glm::vec3 body_a_vel_at_mid =
        body_a.velocity + body_a.angular_velocity * body_a_center_to_mid_perp;

    const glm::vec3 body_b_center_to_mid = midpoint - body_b.position;
    const glm::vec3 body_b_center_to_mid_perp =
        Equations::clockwise_perp_z(body_b_center_to_mid);
    const glm::vec3 body_b_vel_at_mid =
        body_b.velocity + body_b.angular_velocity * body_b_center_to_mid_perp;

    const glm::vec3 relative_vel_at_mid = body_a_vel_at_mid - body_b_vel_at_mid;

    Correction correction_a{};
    Correction correction_b{};
    correction_a.position = std::move(body_a_pos_correction);
    correction_b.position = std::move(body_b_pos_correction);

    // Note: I have never found this to work as expected...
    // If objects are moving away from each other we do not consider it a collision
    /*if (glm::dot(relative_vel_at_mid, ci.normal) <= 0.0) {*/
    /*    return CollisionCorrections{.body_a = std::move(correction_a),*/
    /*                                .body_b = std::move(correction_b)};*/
    /*}*/

    const float impulse_magnitude =
        calculate_impulse_magnitude(ci.normal, relative_vel_at_mid, body_a, body_b,
                                    body_a_center_to_mid, body_b_center_to_mid, c_r);

    const glm::vec3 impulse = impulse_magnitude * ci.normal;

    // Apply linear velocity corrections
    correction_a.velocity = velocity_correction(ci.normal, impulse_magnitude, body_a);
    correction_b.velocity = velocity_correction(ci.normal, -impulse_magnitude, body_b);

    correction_a.angular_velocity =
        angular_velocity_correction(body_a_center_to_mid_perp, impulse, body_a);
    correction_b.angular_velocity =
        angular_velocity_correction(body_b_center_to_mid_perp, -impulse, body_b);

    return CollisionCorrections{.body_a = std::move(correction_a),
                                .body_b = std::move(correction_b)};
}

std::optional<CollisionCorrections> CollisionSolver::resolve_collision_vertex_vertex(
    const CollisionInformation &ci, const RigidBody &body_a, const RigidBody &body_b) {

    glm::vec3 p = ci.contact_patch[ci.deepest_contact_idx];
    glm::vec3 collision_normal = ci.normal;
    float penetration_depth = ci.penetration_depth;

    const glm::vec3 body_a_center_to_p = p - body_a.position;
    const glm::vec3 body_a_center_to_p_perp =
        Equations::clockwise_perp_z(body_a_center_to_p);
    const glm::vec3 body_a_vel_at_p =
        body_a.velocity + body_a.angular_velocity * body_a_center_to_p_perp;

    const glm::vec3 body_b_center_to_p = p - body_b.position;
    const glm::vec3 body_b_center_to_p_perp =
        Equations::clockwise_perp_z(body_b_center_to_p);
    const glm::vec3 body_b_vel_at_p =
        body_b.velocity + body_b.angular_velocity * body_b_center_to_p_perp;

    const glm::vec3 relative_vel_at_p = body_a_vel_at_p - body_b_vel_at_p;

    // Note: I have never found this to work as expected...
    // If objects are moving away from each other we do not consider it a collision
    /*if (glm::dot(relative_vel_at_p, collision_normal) < 0.0) {*/
    /*    return std::nullopt;*/
    /*}*/

    const float c_r = fmin(body_a.collision_restitution, body_b.collision_restitution);

    const float impulse_magnitude =
        calculate_impulse_magnitude(collision_normal, relative_vel_at_p, body_a, body_b,
                                    body_a_center_to_p, body_b_center_to_p, c_r);

    const glm::vec3 body_a_vel_correction =
        velocity_correction(collision_normal, impulse_magnitude, body_a);
    const glm::vec3 body_b_vel_correction =
        velocity_correction(collision_normal, -impulse_magnitude, body_b);

    const glm::vec3 impulse = impulse_magnitude * collision_normal;
    const glm::vec3 neg_impulse = -impulse_magnitude * collision_normal;
    const float body_a_angular_vel_correction =
        angular_velocity_correction(body_a_center_to_p_perp, impulse, body_a);
    const float body_b_angular_vel_correction =
        angular_velocity_correction(body_b_center_to_p_perp, neg_impulse, body_b);

    const glm::vec3 body_a_pos_correction =
        positional_correction(-collision_normal, penetration_depth, body_a, body_b) *
        baumgarte_factor;

    const glm::vec3 body_b_pos_correction =
        positional_correction(collision_normal, penetration_depth, body_b, body_a) *
        baumgarte_factor;

    CollisionCorrections corrections = {
        .body_a =
            {
                .position = std::move(body_a_pos_correction),
                .velocity = std::move(body_a_vel_correction),
                .angular_velocity = body_a_angular_vel_correction,
            },
        .body_b =
            {
                .position = std::move(body_b_pos_correction),
                .velocity = std::move(body_b_vel_correction),
                .angular_velocity = body_b_angular_vel_correction,
            },
    };
    return corrections;
}

std::ostream &operator<<(std::ostream &os, const Correction &c) {
    return os << "Correction( position: " << c.position << ",  velocity: " << c.velocity
              << ", angular_vel: " << c.angular_velocity << ")";
}

std::ostream &operator<<(std::ostream &os, const CollisionCorrections &c) {
    return os << "CollisionCorrections( body_a: " << c.body_a << ",  body_b: " << c.body_b
              << ")";
}

inline glm::vec3 positional_correction(const glm::vec3 &collision_normal,
                                       const float penetration_depth,
                                       const RigidBody &this_body,
                                       const RigidBody &other_body) {
    return (penetration_depth / (this_body.mass + other_body.mass)) * other_body.mass *
           collision_normal;
}

inline float angular_velocity_correction(const glm::vec3 &center_to_p_perp,
                                         const glm::vec3 &impulse,
                                         const RigidBody &body) {
    return glm::dot(center_to_p_perp, impulse) / body.inertia();
}

inline glm::vec3 velocity_correction(const glm::vec3 &normal,
                                     const float impulse_magnitude,
                                     const RigidBody &body) {
    return (impulse_magnitude / body.mass) * normal;
}

inline float calculate_impulse_magnitude(const glm::vec3 &collision_normal,
                                         const glm::vec3 &relative_vel_at_p,
                                         const RigidBody &body_a, const RigidBody &body_b,
                                         const glm::vec3 &body_a_center_to_p,
                                         const glm::vec3 &body_b_center_to_p,
                                         const float c_r) {
    const float nominator = -(1.0 + c_r) * glm::dot(relative_vel_at_p, collision_normal);
    const float denom_term1 =
        glm::dot(collision_normal, collision_normal) * (1.0f / body_a.mass) +
        (1.0f / body_b.mass);
    const float denom_term2 =
        pow(Equations::cross_2d(body_a_center_to_p, collision_normal), 2.0f) /
        body_a.inertia();
    const float denom_term3 =
        pow(Equations::cross_2d(body_b_center_to_p, collision_normal), 2.0f) /
        body_b.inertia();
    return nominator / (denom_term1 + denom_term2 + denom_term3);
}
