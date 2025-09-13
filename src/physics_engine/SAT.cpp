#include "physics_engine/SAT.h"
#include "equations/equations.h"
#include "equations/projection.h"
#include "glm/geometric.hpp"
#include "io.h"
#include "shape.h"
#include <iostream>
#include <optional>
#include <type_traits>
#include <variant>
#include <vector>

struct CollisionEdge {
    glm::vec3 start;
    glm::vec3 end;
    glm::vec3 max;
    glm::vec3 edge;
};

struct MTV {
    glm::vec3 direction;
    float magnitude;
};

std::ostream &operator<<(std::ostream &os, const CollisionEdge &ce);
std::ostream &operator<<(std::ostream &os, const MTV &mtv);
std::ostream &operator<<(std::ostream &os, const ContactType &c);
std::ostream &operator<<(std::ostream &os, const CollisionInformation &ci);

CollisionEdge find_collision_edge(const RigidBody &body, const glm::vec3 &collision_axis);
std::vector<glm::vec3> sat_clip(const glm::vec3 &v1, const glm::vec3 &v2,
                                const glm::vec3 &ref_edge, float offset);
ContactType determine_contact_type(const std::vector<glm::vec3> &clipping_points,
                                   const CollisionEdge &ref_edge,
                                   const CollisionEdge &inc_edge);
CollisionInformation find_clipping_points(const CollisionEdge &edge_a,
                                          const CollisionEdge &edge_b,
                                          const glm::vec3 &collision_normal);
std::optional<MTV> find_mtv(const RigidBody &body_a, const RigidBody &body_b);

std::optional<CollisionInformation> SAT::collision_detection(const RigidBody &body_a,
                                                             const RigidBody &body_b) {

    return std::visit(
        [&body_a, &body_b](const auto &a,
                           const auto &b) -> std::optional<CollisionInformation> {
            using ShapeA = std::decay_t<decltype(a)>;
            using ShapeB = std::decay_t<decltype(b)>;

            if constexpr (std::is_same_v<ShapeA, Circle> &&
                          std::is_same_v<ShapeB, Circle>) {
                return SAT::collision_detection_circle(body_a, body_b);
                return std::nullopt;
            } else if constexpr (std::is_same_v<ShapeA, Circle> &&
                                 !std::is_same_v<ShapeB, Circle>) {
                return SAT::collision_detection_circle_polygon(body_a, body_b);
                return std::nullopt;
            } else if (!std::is_same_v<ShapeA, Circle> &&
                       std::is_same_v<ShapeB, Circle>) {
                return SAT::collision_detection_circle_polygon(body_b, body_a);
            } else {
                return SAT::collision_detection_polygon(body_a, body_b);
            }
        },
        body_a.shape.params, body_b.shape.params);
}

std::ostream &operator<<(std::ostream &os, const CollisionEdge &ce) {
    os << "CollisionEdge( start: " << ce.start << ", end: " << ce.end
       << ", max: " << ce.max << " , edge: " << ce.edge << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const MTV &mtv) {
    os << "MTV( direction: " << mtv.direction << ", magnitude: " << mtv.magnitude << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const ContactType &c) {
    switch (c) {
    case ContactType::VERTEX_VERTEX:
        return os << "ContactType::VERTEX_VERTEX";
    case ContactType::VERTEX_EDGE:
        return os << "ContactType::VERTEX_EDGE";
    case ContactType::EDGE_EDGE:
        return os << "ContactType::EDGE_EDGE";
    default:
        return os << "ContactType::NONE";
    }
}

std::ostream &operator<<(std::ostream &os, const CollisionInformation &ci) {
    os << "CollisionInformation( contact_type: " << ci.contact_type
       << ", contact_patch: " << ci.contact_patch << ", normal: " << ci.normal
       << ", depth: " << ci.penetration_depth
       << ", deepest_contact_idx: " << ci.deepest_contact_idx << " )";
    return os;
}

// Find the edge most aligned with the collision axis
CollisionEdge find_collision_edge(const RigidBody &body,
                                  const glm::vec3 &collision_axis) {
    auto vertices = body.vertices();

    // Find vertex with maximum projection along collision axis
    int index = 0;
    float max_proj = -std::numeric_limits<float>::infinity();

    for (size_t i = 0; i < vertices.size(); ++i) {
        float proj = glm::dot(collision_axis, vertices[i]);
        if (proj > max_proj) {
            max_proj = proj;
            index = i;
        }
    }

    const int num_vertices = vertices.size();
    const glm::vec3 prev_vertex = vertices[(index - 1 + num_vertices) % num_vertices];
    const glm::vec3 mid_vertex = vertices[index];
    const glm::vec3 next_vertex = vertices[(index + 1 + num_vertices) % num_vertices];

    // Be careful when computing the left and right (l and r in the code above) vectors as
    // they both must point towards the maximum point
    glm::vec3 left_edge = mid_vertex - next_vertex;
    glm::vec3 right_edge = mid_vertex - prev_vertex;

    // Normalize edge vectors
    left_edge = glm::normalize(left_edge);
    right_edge = glm::normalize(right_edge);

    // We determine the closest as the edge who is most perpendicular to the separation
    // normal.
    if (glm::dot(right_edge, collision_axis) <= glm::dot(left_edge, collision_axis)) {
        return CollisionEdge{.start = prev_vertex,
                             .end = mid_vertex,
                             .max = mid_vertex,
                             .edge = mid_vertex - prev_vertex};
    } else {
        return CollisionEdge{.start = mid_vertex,
                             .end = next_vertex,
                             .max = mid_vertex,
                             .edge = next_vertex - mid_vertex};
    }
}

std::vector<glm::vec3> sat_clip(const glm::vec3 &v1, const glm::vec3 &v2,
                                const glm::vec3 &ref_edge, float offset) {
    std::vector<glm::vec3> clipped_points;

    float d1 = glm::dot(ref_edge, v1) - offset;
    float d2 = glm::dot(ref_edge, v2) - offset;

    // Add points that are inside (positive side of plane)
    if (d1 >= 0.0f) {
        clipped_points.push_back(v1);
    }
    if (d2 >= 0.0f) {
        clipped_points.push_back(v2);
    }

    // If points are on opposite sides of the plane, add intersection point
    if (d1 * d2 < 0.0f) {
        float u = d1 / (d1 - d2);
        glm::vec3 e = v1 + u * (v2 - v1);
        clipped_points.push_back(e);
    }

    return clipped_points;
}

ContactType determine_contact_type(const std::vector<glm::vec3> &clipping_points,
                                   const CollisionEdge &ref_edge,
                                   const CollisionEdge &inc_edge) {

    if (clipping_points.size() > 1) {
        // Dot product close to 1 or -1 indicates parallel edges (edges are already
        // normalized)
        const float edge_alignment = std::abs(glm::dot(ref_edge.edge, inc_edge.edge));
        constexpr float alignment_threshold = 0.996; // Roughly within +-5 degrees
        if (edge_alignment > alignment_threshold) {
            return ContactType::EDGE_EDGE;
        }
    }

    if (clipping_points.size() == 1) {
        const glm::vec3 &point = clipping_points[0];
        // Clipping point is always on the incident edge, therefor we only check for
        // proximity to reference edge start and stop
        const float distance_start = Equations::distance2(point, ref_edge.start);
        const float distance_end = Equations::distance2(point, ref_edge.end);
        constexpr float distance_threshold = 0.01f;
        if (distance_start < distance_threshold || distance_end < distance_threshold) {
            return ContactType::VERTEX_VERTEX;
        }
    }
    return ContactType::VERTEX_EDGE;
}

// Find clipping points between two bodies in collision
CollisionInformation find_clipping_points(const CollisionEdge &edge_a,
                                          const CollisionEdge &edge_b,
                                          const glm::vec3 &collision_normal) {
    // Choose reference edge (the one most perpendicular to collision normal)
    bool flip = false;
    CollisionEdge reference_edge, incident_edge;

    if (std::abs(glm::dot(edge_a.edge, collision_normal)) <=
        std::abs(glm::dot(edge_b.edge, collision_normal))) {
        reference_edge = edge_a;
        incident_edge = edge_b;
    } else {
        reference_edge = edge_b;
        incident_edge = edge_a;
        flip = true;
    }

    reference_edge.edge = glm::normalize(reference_edge.edge);
    incident_edge.edge = glm::normalize(incident_edge.edge);

    const float offset_1 = glm::dot(reference_edge.edge, reference_edge.start);
    std::vector<glm::vec3> clipped_points =
        sat_clip(incident_edge.start, incident_edge.end, reference_edge.edge, offset_1);

    if (clipped_points.size() < 2) {
        return {};
    }

    const float offset_2 = glm::dot(reference_edge.edge, reference_edge.end);
    clipped_points =
        sat_clip(clipped_points[0], clipped_points[1], -reference_edge.edge, -offset_2);

    if (clipped_points.size() < 2) {
        return {};
    }

    const glm::vec3 reference_edge_norm =
        Equations::counterclockwise_perp_z(reference_edge.edge);
    const float max = glm::dot(reference_edge_norm, reference_edge.max);

    // Final clipping
    float max_depth = 0.0;
    size_t max_depth_idx = 0;
    std::vector<glm::vec3> contact_patch;
    for (size_t i = 0; i < clipped_points.size(); i++) {
        float depth = glm::dot(reference_edge_norm, clipped_points[i]) - max;
        if (depth >= 0.0f) {
            contact_patch.push_back(std::move(clipped_points[i]));
            if (max_depth < depth) {
                max_depth = depth;
                max_depth_idx = contact_patch.size() - 1;
            }
        }
    }

    ContactType contact_type =
        determine_contact_type(contact_patch, reference_edge, incident_edge);

    return CollisionInformation{
        .penetration_depth = max_depth,
        .normal = std::move(collision_normal),
        .contact_type = contact_type,
        .contact_patch = std::move(contact_patch),
        .deepest_contact_idx = max_depth_idx,
    };
}

inline std::optional<MTV> find_mtv_circle(const RigidBody &body_a,
                                          const RigidBody &body_b) {
    const auto radius_a = body_a.shape.get<Circle>().diameter / 2.0f;
    const auto radius_b = body_b.shape.get<Circle>().diameter / 2.0f;
    const float distance2 = Equations::distance2(body_b.position, body_a.position);
    if (distance2 >= pow(radius_a + radius_b, 2)) {
        return std::nullopt;
    }
    const float distance = sqrt(distance2);
    const float overlap = radius_a + radius_b - distance;
    return MTV{.direction = glm::normalize(body_b.position - body_a.position),
               .magnitude = overlap};
}

inline std::optional<MTV> find_mtv_circle_polygon(const RigidBody &circle,
                                                  const RigidBody &polygon) {

    std::vector<glm::vec3> polygon_axis = polygon.normals();
    float min_overlap = std::numeric_limits<float>::infinity();
    glm::vec3 axis;

    for (size_t i = 0; i < polygon_axis.size(); ++i) {
        Projection poly_proj =
            Projection::project_polygon_on_axis(polygon, polygon_axis[i]);
        Projection circle_proj =
            Projection::project_circle_on_axis(circle, polygon_axis[i]);
        Overlap overlap = poly_proj.overlap(circle_proj);

        if (overlap.distance <= 0.0f) {
            return std::nullopt; // No collision
        }

        if (overlap.distance < min_overlap) {
            min_overlap = overlap.distance;
            axis = polygon_axis[i];
        }
    }

    return MTV{.direction = -axis, .magnitude = min_overlap};
}

inline std::optional<MTV> find_mtv_polygon(const RigidBody &body_a,
                                           const RigidBody &body_b) {

    std::vector<glm::vec3> axii_a = body_a.normals();
    std::vector<glm::vec3> axii_b = body_b.normals();

    float min_overlap = std::numeric_limits<float>::infinity();
    glm::vec3 axis;

    for (size_t i = 0; i < axii_a.size(); ++i) {
        Projection proj_a = Projection::project_polygon_on_axis(body_a, axii_a[i]);
        Projection proj_b = Projection::project_polygon_on_axis(body_b, axii_a[i]);
        Overlap overlap = proj_a.overlap(proj_b);

        if (overlap.distance <= 0.0f) {
            return std::nullopt; // No collision
        }

        if (overlap.distance < min_overlap) {
            min_overlap = overlap.distance;
            axis = axii_a[i];
        }
    }

    for (size_t i = 0; i < axii_b.size(); ++i) {
        Projection proj_a = Projection::project_polygon_on_axis(body_a, axii_b[i]);
        Projection proj_b = Projection::project_polygon_on_axis(body_b, axii_b[i]);
        Overlap overlap = proj_a.overlap(proj_b);

        if (overlap.distance <= 0.0f) {
            return std::nullopt; // No collision
        }

        if (overlap.distance < min_overlap) {
            min_overlap = overlap.distance;
            axis = axii_b[i];
        }
    }

    glm::vec3 direction_a_to_b = body_b.position - body_a.position;
    glm::vec3 final_axis = axis;
    if (glm::dot(final_axis, direction_a_to_b) < 0) {
        final_axis = -final_axis;
    }
    return MTV{.direction = final_axis, .magnitude = min_overlap};
}

inline std::optional<CollisionInformation>
SAT::collision_detection_circle(const RigidBody &body_a, const RigidBody &body_b) {
    const auto mtv = find_mtv_circle(body_a, body_b);
    if (!mtv.has_value()) {
        return std::nullopt;
    }
    const auto radius_a = body_a.shape.get<Circle>().diameter / 2.0f;
    return CollisionInformation{
        .penetration_depth = mtv->magnitude,
        .normal = mtv->direction,
        .contact_type = ContactType::VERTEX_VERTEX,
        .contact_patch = {body_a.position + mtv->direction * radius_a},
        .deepest_contact_idx = 0};
}

inline std::optional<CollisionInformation>
SAT::collision_detection_circle_polygon(const RigidBody &circle,
                                        const RigidBody &polygon) {

    const auto mtv = find_mtv_circle_polygon(circle, polygon);
    if (!mtv.has_value()) {
        return std::nullopt;
    }
    const float radius = circle.shape.get<Circle>().diameter / 2.0f;
    return CollisionInformation{
        .penetration_depth = mtv->magnitude,
        .normal = mtv->direction,
        .contact_type = ContactType::VERTEX_VERTEX,
        .contact_patch = {circle.position + mtv->direction * radius},
        .deepest_contact_idx = 0};
}

inline std::optional<CollisionInformation>
SAT::collision_detection_polygon(const RigidBody &body_a, const RigidBody &body_b) {
    const auto mtv = find_mtv_polygon(body_a, body_b);
    if (!mtv.has_value()) {
        return std::nullopt;
    }

    const glm::vec3 collision_normal = mtv.value().direction;

    auto edge_a = find_collision_edge(body_a, collision_normal);
    auto edge_b = find_collision_edge(body_b, -collision_normal);

    CollisionInformation info = find_clipping_points(edge_a, edge_b, collision_normal);

    if (info.contact_patch.empty()) {
        return std::nullopt;
    }

    return info;
}
