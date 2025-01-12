#include "physics_engine/SAT.h"
#include "equations/equations.h"
#include "equations/projection.h"
#include "glm/geometric.hpp"
#include "io.h"
#include <algorithm>
#include <iostream>
#include <optional>
#include <vector>

struct CollisionEdge {
    glm::vec3 start;
    glm::vec3 end;
    glm::vec3 max;
    glm::vec3 edge;
};

struct ClippedPoint {
    glm::vec3 vertex;
    float depth;
};

struct MTV {
    glm::vec3 direction;
    float magnitude;
};

std::ostream &operator<<(std::ostream &os, const ClippedPoint &cp) {
    os << "ClippedPoint( vertex: " << cp.vertex << ", depth: " << cp.depth << ")";
    return os;
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

// Find the edge most aligned with the collision axis
CollisionEdge sat_find_collision_edge(const RigidBody &body,
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

    /*std::cout << std::endl;*/
    /*std::cout << "Collision axis: " << collision_axis << std::endl;*/
    /*std::cout << "Index: " << index << std::endl;*/
    /*std::cout << std::endl;*/

    const int num_vertices = vertices.size();
    const glm::vec3 prev_vertex = vertices[(index - 1 + num_vertices) % num_vertices];
    const glm::vec3 mid_vertex = vertices[index];
    const glm::vec3 next_vertex = vertices[(index + 1 + num_vertices) % num_vertices];

    /*std::cout << std::endl;*/
    /*std::cout << "prev_vert: " << prev_vertex << std::endl;*/
    /*std::cout << "mid_vert: " << mid_vertex << std::endl;*/
    /*std::cout << "next_vert: " << next_vertex << std::endl;*/
    /*std::cout << std::endl;*/

    // Be careful when computing the left and right (l and r in the code above) vectors as
    // they both must point towards the maximum point
    glm::vec3 left_edge = mid_vertex - next_vertex;
    glm::vec3 right_edge = mid_vertex - prev_vertex;

    /*std::cout << std::endl;*/
    /*std::cout << "left edge: " << left_edge << std::endl;*/
    /*std::cout << "right edge: " << right_edge << std::endl;*/
    /*std::cout << std::endl;*/

    // Normalize edge vectors
    left_edge = glm::normalize(left_edge);
    right_edge = glm::normalize(right_edge);

    // We determine the closest as the edge who is most perpendicular to the separation
    // normal.
    if (glm::dot(right_edge, collision_axis) <= glm::dot(left_edge, collision_axis)) {
        return CollisionEdge{.max = mid_vertex,
                             .start = prev_vertex,
                             .end = mid_vertex,
                             .edge = mid_vertex - prev_vertex};
    } else {
        return CollisionEdge{.max = mid_vertex,
                             .start = mid_vertex,
                             .end = next_vertex,
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

// Find clipping points between two bodies in collision
std::vector<ClippedPoint> sat_find_clipping_points(const RigidBody &body_a,
                                                   const RigidBody &body_b,
                                                   const glm::vec3 &collision_normal) {
    auto edge_a = sat_find_collision_edge(body_a, collision_normal);
    auto edge_b = sat_find_collision_edge(body_b, -collision_normal);

    /*std::cout << "Body A: " << edge_a << std::endl;*/
    /*std::cout << "Body B: " << edge_b << std::endl;*/

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

    /*std::cout << "Reference edge: " << reference_edge << std::endl;*/
    /*std::cout << "Incident edge: " << incident_edge << std::endl;*/

    reference_edge.edge = glm::normalize(reference_edge.edge);

    float offset_1 = glm::dot(reference_edge.edge, reference_edge.start);
    std::vector<glm::vec3> clipped_points =
        sat_clip(incident_edge.start, incident_edge.end, reference_edge.edge, offset_1);

    if (clipped_points.size() < 2) {
        return {};
    }

    float offset_2 = glm::dot(reference_edge.edge, reference_edge.end);
    clipped_points =
        sat_clip(clipped_points[0], clipped_points[1], -reference_edge.edge, -offset_2);

    if (clipped_points.size() < 2) {
        return {};
    }

    glm::vec3 reference_edge_norm =
        Equations::counterclockwise_perp_z(reference_edge.edge);
    float max = glm::dot(reference_edge_norm, reference_edge.max);

    std::vector<ClippedPoint> result;
    for (const auto &point : clipped_points) {
        float depth = glm::dot(reference_edge_norm, point) - max;
        if (depth >= 0.0f) {
            result.push_back(ClippedPoint{.vertex = point, .depth = depth});
        }
    }

    return result;
}

std::optional<MTV> find_mtv(const RigidBody &body_a, const RigidBody &body_b) {
    std::vector<glm::vec3> axii_a = body_a.normals();
    std::vector<glm::vec3> axii_b = body_b.normals();

    float min_overlap = std::numeric_limits<float>::infinity();
    std::optional<glm::vec3> axis = std::nullopt;

    for (size_t i = 0; i < axii_a.size(); ++i) {
        Projection proj_a = Projection::project_body_on_axis(body_a, axii_a[i]);
        Projection proj_b = Projection::project_body_on_axis(body_b, axii_a[i]);
        Overlap overlap = proj_a.overlap(proj_b);

        if (overlap.distance <= 0.0f) {
            return std::nullopt; // No collision
        }

        if (overlap.distance < min_overlap) {
            min_overlap = overlap.distance;
            axis = axii_a[i];
            /*min_index_a = i;*/
        }
    }

    for (size_t i = 0; i < axii_b.size(); ++i) {
        Projection proj_a = Projection::project_body_on_axis(body_a, axii_b[i]);
        Projection proj_b = Projection::project_body_on_axis(body_b, axii_b[i]);
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
    glm::vec3 final_axis = axis.value();
    if (glm::dot(final_axis, direction_a_to_b) < 0) {
        final_axis = -final_axis;
    }
    return MTV{.direction = final_axis, .magnitude = min_overlap};
}

std::optional<CollisionInformation> SAT::collision_detection(const RigidBody &body_a,
                                                             const RigidBody &body_b) {
    auto mtv = find_mtv(body_a, body_b);
    if (!mtv.has_value()) {
        /*std::cout << "No collision" << std::endl;*/
        return std::nullopt;
    }
    glm::vec3 collision_normal = mtv.value().direction;

    std::vector<ClippedPoint> clipping_points =
        sat_find_clipping_points(body_a, body_b, collision_normal);

    // Find deepest penetration point
    auto deepest_point = std::max_element(
        clipping_points.begin(), clipping_points.end(),
        [](const ClippedPoint &a, const ClippedPoint &b) { return a.depth < b.depth; });

    if (deepest_point == clipping_points.end()) {
        return std::nullopt;
    }

    return CollisionInformation{.penetration_depth = deepest_point->depth,
                                .normal = collision_normal,
                                .collision_point = deepest_point->vertex};
}
