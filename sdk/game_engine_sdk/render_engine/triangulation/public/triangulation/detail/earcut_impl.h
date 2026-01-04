#pragma once

#include "ads/DoubleLinkedList.h"
#include "math/area.h"
#include <cfloat>
#include <vector>
namespace triangulation::detail {

template <typename T> class EarcutImpl {
  public:
    static std::pair<T, T> make_vector(const std::pair<T, T> &from,
                                       const std::pair<T, T> &to) {
        return std::make_pair<float, float>(to.first - from.first,
                                            to.second - from.second);
    }

    static T cross_prod(const std::pair<T, T> &va, const std::pair<T, T> &vb) {
        return va.first * vb.second - va.second * vb.first;
    }

    static bool is_convex(const std::pair<T, T> &v_prev, const std::pair<T, T> &v_curr,
                          const std::pair<T, T> &v_next) {
        const std::pair<float, float> va = make_vector(v_curr, v_prev);
        const std::pair<float, float> vb = make_vector(v_curr, v_next);
        const float cross = cross_prod(va, vb);
        return cross < 0.0f;
    }

    static bool is_ear(const std::pair<T, T> &v_prev, const std::pair<T, T> &v_curr,
                       const std::pair<T, T> &v_next,
                       const std::vector<std::pair<T, T>> &vertices,
                       const ads::DoubleLinkedList<size_t> &reflex_vertices) {

        // Create edges pointing in a counter clockwise order
        const std::pair<T, T> vab = make_vector(v_prev, v_curr);
        const std::pair<T, T> vbc = make_vector(v_curr, v_next);
        const std::pair<T, T> vca = make_vector(v_next, v_prev);

        for (const size_t &other_vertice : reflex_vertices) {
            const auto &p = vertices[other_vertice];

            if (p == v_prev || p == v_curr || p == v_next) {
                continue;
            }

            const std::pair<T, T> vpa = make_vector(p, v_prev);
            const std::pair<T, T> vpb = make_vector(p, v_curr);
            const std::pair<T, T> vpc = make_vector(p, v_next);

            // Containment test
            const T cross_a = cross_prod(vab, vpa);
            const T cross_b = cross_prod(vbc, vpb);
            const T cross_c = cross_prod(vca, vpc);
            if (cross_a <= 0.0f && cross_b <= 0.0f && cross_c <= 0.0f) {
                // point p is inside the triangle
                return false;
            }
        }
        return true;
    }

    static std::vector<std::pair<T, T>> bridge_exterior_and_interior(
        const std::vector<std::pair<T, T>> &exterior,
        const std::vector<std::vector<std::pair<T, T>>> &interiors) {

        if (interiors.size() == 0) {
            return exterior;
        }

        std::vector<std::pair<T, T>> bridged_vertices = exterior;

        // Given multiple inner polygons, the one containing the vertex of maximum
        // x-value of all inner polygon vertices is the one chosen to combine with the
        // outer polygon. The process is then repeated with the new outer polygon and the
        // remaining inner polygons.
        std::vector<std::tuple<T, size_t, size_t>> max_xs;
        max_xs.reserve(interiors.size());
        for (size_t interior_idx = 0; interior_idx < interiors.size(); interior_idx++) {
            const auto &interior = interiors[interior_idx];
            DEBUG_CODE({
                const bool exterior_winding = math::signed_area(exterior) < 0.0f;
                const bool interior_winding = math::signed_area(interior) < 0.0f;
                DEBUG_ASSERT(
                    exterior_winding != interior_winding,
                    "Error: Exterior and interior winding orders need to be different.");
            });

            DEBUG_ASSERT(interior.size() != 0,
                         "Error: Did not expect an internal polygon with no vertices.");

            const size_t largest_x_index = find_vertex_with_largest_x(interior);
            const T largest_x = interior[largest_x_index].first;
            max_xs.emplace_back(largest_x, interior_idx, largest_x_index);
        }

        std::ranges::sort(max_xs);

        for (const std::tuple<T, size_t, size_t> &values : max_xs) {
            const size_t interior_idx = std::get<1>(values);
            const size_t max_x_idx = std::get<2>(values);
            const std::vector<std::pair<T, T>> interior = interiors[interior_idx];

            // Get the interior vertex with the larges x
            const size_t interior_bridge_vertex = max_x_idx;
            const std::pair<T, T> &M = interior[interior_bridge_vertex];

            const std::pair<size_t, std::pair<float, float>> intersection_point =
                find_nearest_intersecting_edge(M, bridged_vertices);
            const size_t closest_edge_id = intersection_point.first;
            const std::pair<float, float> I = intersection_point.second;

            DEBUG_ASSERT(closest_edge_id < bridged_vertices.size(),
                         "Error: Failed to find intersection edge.");

            const std::pair<float, float> &start = bridged_vertices[closest_edge_id];
            const std::pair<float, float> &end =
                bridged_vertices[(closest_edge_id + 1) % bridged_vertices.size()];

            // If I is a vertex of the outer polygon, then M and I are mutually
            // visible and the algorithm terminates
            if (abs(I.first - start.first) < std::numeric_limits<float>::epsilon() &&
                abs(I.second - start.second) < std::numeric_limits<float>::epsilon()) {

                bridged_vertices = bridge(bridged_vertices, interior, closest_edge_id,
                                          interior_bridge_vertex);
                continue;
            }

            if (abs(I.first - end.first) < std::numeric_limits<float>::epsilon() &&
                abs(I.second - end.second) < std::numeric_limits<float>::epsilon()) {
                bridged_vertices = bridge(bridged_vertices, interior,
                                          (closest_edge_id + 1) % bridged_vertices.size(),
                                          interior_bridge_vertex);
                continue;
            }

            //  Select the vertex from the edge with the largest X
            size_t candidate_exterior_bridge_vertex =
                start.first >= end.first
                    ? closest_edge_id
                    : (closest_edge_id + 1) % bridged_vertices.size();

            const std::pair<T, T> &P = bridged_vertices[candidate_exterior_bridge_vertex];

            // Search the reflex vertices of the outer polygon, not including P if it
            // happens to be reflex. If all of these vertices are strictly outside
            // triangle ⟨M, I, P ⟩, then M and P are mutually visible and the
            // algorithm terminates.
            const ads::DoubleLinkedList<size_t> exterior_reflex_vertices =
                find_reflex_vertices(bridged_vertices);
            const bool I_is_ear =
                is_ear(P, I, M, bridged_vertices, exterior_reflex_vertices);
            if (I_is_ear) {
                bridged_vertices =
                    bridge(bridged_vertices, interior, candidate_exterior_bridge_vertex,
                           interior_bridge_vertex);
                continue;
            }

            // Otherwise, at least one reflex vertex lies in ⟨M, I, P ⟩. Search
            // for the reflex R that minimizes the angle between ⟨M, I⟩ and ⟨M, R⟩;
            // then M and R are mutually visible and the algorithm terminates.
            candidate_exterior_bridge_vertex =
                find_exterior_reflex_vertex_with_smallest_angle(M, I, bridged_vertices,
                                                                exterior_reflex_vertices);

            bridged_vertices =
                bridge(bridged_vertices, interior, candidate_exterior_bridge_vertex,
                       interior_bridge_vertex);
        }

        return bridged_vertices;
    }

    static ads::DoubleLinkedList<size_t>
    find_reflex_vertices(const std::vector<std::pair<T, T>> &vertices) {
        ads::DoubleLinkedList<size_t> reflex_vertices;
        for (size_t i = 0; i < vertices.size(); i++) {
            const auto prev = vertices[(i + vertices.size() - 1) % vertices.size()];
            const auto curr = vertices[i];
            const auto next = vertices[i + 1];
            if (!is_convex(prev, curr, next)) {
                reflex_vertices.push_back(i);
            }
        }
        return reflex_vertices;
    }

    static size_t find_exterior_reflex_vertex_with_smallest_angle(
        const std::pair<float, float> &M, const std::pair<float, float> &I,
        const std::vector<std::pair<T, T>> &exterior,
        const ads::DoubleLinkedList<size_t> &exterior_reflex_vertices) {

        const std::pair<float, float> v_mi = make_vector(M, I);
        const float len_mi_sq = v_mi.first * v_mi.first + v_mi.second * v_mi.second;
        float max_cos =
            -std::numeric_limits<float>::max(); // smallest angle = largest cosine
        size_t best_reflex_idx = exterior_reflex_vertices.front();
        for (const size_t &i : exterior_reflex_vertices) {
            const std::pair<float, float> R = exterior[i];
            const std::pair<float, float> v_mr = make_vector(M, R);
            float dot = v_mi.first * v_mr.first + v_mi.second * v_mr.second;
            float len_mr_sq = v_mr.first * v_mr.first + v_mr.second * v_mr.second;

            // cos(angle) = dot / (len_mi * len_mr)
            // Squaring both sides: cos²(angle) = dot² / (len_mi² * len_mr²)
            // To compare: cos²(angle) * len_mi² * len_mr² = dot²
            // But we want largest cos (smallest angle), so compare: dot² / (len_mi² *
            // len_mr²)

            float cos_angle_sq = (dot * dot) / (len_mi_sq * len_mr_sq);

            if (cos_angle_sq > max_cos) {
                max_cos = cos_angle_sq;
                best_reflex_idx = i;
            }
        }
        return best_reflex_idx;
    }

    static std::vector<std::pair<T, T>> bridge(const std::vector<std::pair<T, T>> &v1,
                                               const std::vector<std::pair<T, T>> &v2,
                                               const size_t split,
                                               const size_t v2_start) {

        std::vector<std::pair<T, T>> new_bridged_vertices;
        new_bridged_vertices.reserve(v1.size() + v2.size() + 2);

        for (size_t i = 0; i < v1.size(); i++) {
            new_bridged_vertices.push_back(v1[i]);
            if (i == split) {
                for (size_t j = 0; j < v2.size(); j++) {
                    new_bridged_vertices.push_back(v2[(j + v2_start) % v2.size()]);
                }
                // We need to start and end with a bridge to and from the interior
                new_bridged_vertices.push_back(v2[v2_start]);
                new_bridged_vertices.push_back(v1[i]);
            }
        }
        return new_bridged_vertices;
    }

    static size_t
    find_vertex_with_largest_x(const std::vector<std::pair<T, T>> &vertices) {
        DEBUG_ASSERT(vertices.size() > 0, "Error: Did not expect empty vector.");
        size_t interior_bridge_vertex = 0;
        T max_interior_x_found = vertices[0].first;
        for (size_t i = 1; i < vertices.size(); i++) {
            if (vertices[i].first > max_interior_x_found) {
                max_interior_x_found = vertices[i].first;
                interior_bridge_vertex = i;
            }
        }
        return interior_bridge_vertex;
    }

    static std::pair<size_t, std::pair<T, T>>
    find_nearest_intersecting_edge(const std::pair<T, T> &M,
                                   const std::vector<std::pair<T, T>> &vertices) {

        float min_x = FLT_MAX;
        size_t closest_edge_id = vertices.size();
        std::pair<float, float> intersection_point = {
            FLT_MAX, FLT_MAX}; // Store the intersection point

        for (size_t i = 0; i < vertices.size(); i++) {
            const auto &start = vertices[i];
            const auto &end = vertices[(i + 1) % vertices.size()];

            if (M.first >= start.first && M.first >= end.first) {
                continue;
            }
            if (start.second > M.second || end.second < M.second) {
                continue;
            }

            // Check for horizontal edge
            if (std::abs(end.second - start.second) < std::numeric_limits<T>::epsilon()) {
                continue;
            }

            // Intersection test
            float t_param = static_cast<float>(M.second - start.second) /
                            static_cast<float>(end.second - start.second);
            float intersection_x =
                start.first + t_param * static_cast<float>(end.first - start.first);

            if (intersection_x < M.first) {
                // Intersection is to the left of M
                continue;
            }

            if (intersection_x < min_x) {
                min_x = intersection_x;
                closest_edge_id = i;
                // Store the intersection point
                intersection_point = {intersection_x, M.second};
            }
        }
        return std::make_pair(closest_edge_id, std::move(intersection_point));
    }
};
} // namespace triangulation::detail
