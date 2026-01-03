#pragma once

#include "ads/DoubleLinkedList.h"
#include "math/area.h"
#include "triangulation/Triangles.h"
#include "util/assert.h"
#include <cfloat>
#include <vector>
namespace triangulation {

template <typename T> class Earcut {
  public:
    static Triangles<T>
    run(const std::vector<std::pair<T, T>> &bridged_vertices,
        const std::vector<std::vector<std::pair<T, T>>> &interior_vertices) {
        Earcut<T> ec;
        return ec.run_(bridged_vertices, interior_vertices);
    }

  private:
    std::vector<bool> m_is_reflex_vertex;
    ads::DoubleLinkedList<size_t> m_reflex_vertices;

    std::vector<bool> m_is_removed;
    std::vector<bool> m_is_ear_vertex;
    /*ads::DoubleLinkedList<size_t> m_ear_vertices;*/

    std::vector<size_t> m_next_id;
    std::vector<size_t> m_prev_id;

    Earcut() = default;
    Triangles<T>
    run_(const std::vector<std::pair<T, T>> &exterior_vertices,
         const std::vector<std::vector<std::pair<T, T>>> &interior_vertices) {

        if (exterior_vertices.size() < 3) {
            return Triangles<T>{};
        }

        const std::vector<std::pair<T, T>> bridged_vertices =
            bridge_exterior_and_interior(exterior_vertices, interior_vertices);

        m_is_reflex_vertex.resize(bridged_vertices.size());
        m_is_removed.resize(bridged_vertices.size());
        m_next_id.resize(bridged_vertices.size());
        m_prev_id.resize(bridged_vertices.size());

        // Classify vertices as convex or reflex
        const size_t bridged_vertices_size = bridged_vertices.size();
        for (auto i = 0; i < bridged_vertices.size(); i++) {
            // Populate the prev and next vectors
            m_prev_id[i] = (i + bridged_vertices_size - 1) % bridged_vertices_size;
            m_next_id[i] = (i + 1) % bridged_vertices_size;

            const bool is_conv =
                is_convex(bridged_vertices[m_prev_id[i]], bridged_vertices[i],
                          bridged_vertices[m_next_id[i]]);

            m_is_reflex_vertex[i] = !is_conv;

            if (!is_conv) {
                m_reflex_vertices.push_back(i);
            }

            m_is_removed[i] = false;
        }

        // Determine which bridged_vertices are m_is_ear_vertex
        m_is_ear_vertex.resize(bridged_vertices.size());
        for (size_t i = 0; i < m_is_reflex_vertex.size(); i++) {
            if (m_is_reflex_vertex[i]) {
                continue;
            }

            m_is_ear_vertex[i] = is_ear(
                bridged_vertices[m_prev_id[i]], bridged_vertices[i],
                bridged_vertices[m_next_id[i]], bridged_vertices, m_reflex_vertices);
            /*m_ear_vertices.push_back(i);*/

            DEBUG_CODE({
                if (m_is_ear_vertex[i]) {
                    DEBUG_ASSERT(!m_is_reflex_vertex[i],
                                 "Error: all ear vertices are ear vertices.");
                }
            });
        }

        DEBUG_ASSERT(m_is_ear_vertex.size() > 0, "Error: Found no ears in the polygon.");

        std::vector<std::array<size_t, 3>> triangles;
        triangles.reserve(bridged_vertices.size() - 2);
        size_t i = 0;
        // IMPROVEMENT: Use doubly linked list for ears and pop from front when removing
        // indices. This would save some iterations where i is not an ear.
        while (triangles.size() < bridged_vertices.size() - 2) {
            if (!m_is_ear_vertex[i] || m_is_removed[i]) {
                i = (i + 1) % bridged_vertices.size();
                continue;
            }
            /*const size_t i = m_ear_vertices.front();*/
            /*m_ear_vertices.pop_front();*/

            triangles.emplace_back(std::array{m_prev_id[i], i, m_next_id[i]});

            if (triangles.size() >= bridged_vertices.size() - 2) {
                break;
            }

            m_next_id[m_prev_id[i]] = m_next_id[i];
            m_prev_id[m_next_id[i]] = m_prev_id[i];
            m_is_removed[i] = true;
            m_is_ear_vertex[i] = false;
            m_is_reflex_vertex[i] = false;

            reclassify_vertex(m_prev_id[i], bridged_vertices);
            reclassify_vertex(m_next_id[i], bridged_vertices);

            i = (i + 1) % bridged_vertices.size();
        }

        return Triangles<T>{.vertices = std::move(bridged_vertices),
                            .indices = std::move(triangles)};
    }

    void reclassify_vertex(const size_t i, const std::vector<std::pair<T, T>> &vertices) {

        const auto v_prev = vertices[m_prev_id[i]];
        const auto v_curr = vertices[i];
        const auto v_next = vertices[m_next_id[i]];

        const bool now_conv = is_convex(v_prev, v_curr, v_next);
        if (m_is_reflex_vertex[i]) {
            m_is_reflex_vertex[i] = !now_conv;
            if (now_conv) {
                m_reflex_vertices.remove(i);
            }
        }

        // Wether the vertex became an ear or remain an ear after the adjacent vertex was
        // removed, if it is convex we need to check if it is an ear
        if (now_conv) {
            /*const bool was_ear = m_is_ear_vertex[i];*/
            const bool now_ear =
                is_ear(v_prev, v_curr, v_next, vertices, m_reflex_vertices);
            m_is_ear_vertex[i] = now_ear;

            /*if (!was_ear && now_ear) {*/
            /*    m_ear_vertices.push_back(i);*/
            /*} else if (was_ear && !now_ear) {*/
            /*    m_ear_vertices.remove(i);*/
            /*}*/
        }

        DEBUG_CODE({
            if (m_is_ear_vertex[i]) {
                DEBUG_ASSERT(!m_is_reflex_vertex[i],
                             "Error: all ear vertices are ear vertices.");
            }
        });
    }

    bool is_ear(const std::pair<T, T> &v_prev, const std::pair<T, T> &v_curr,
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

    bool is_convex(const std::pair<T, T> &v_prev, const std::pair<T, T> &v_curr,
                   const std::pair<T, T> &v_next) {
        const std::pair<float, float> va = make_vector(v_curr, v_prev);
        const std::pair<float, float> vb = make_vector(v_curr, v_next);
        const float cross = cross_prod(va, vb);
        return cross < 0.0f;
    }

    std::pair<T, T> make_vector(const std::pair<T, T> &from, const std::pair<T, T> &to) {
        return std::make_pair<float, float>(to.first - from.first,
                                            to.second - from.second);
    }

    T cross_prod(const std::pair<T, T> &va, const std::pair<T, T> &vb) {
        return va.first * vb.second - va.second * vb.first;
    }

    std::vector<std::pair<T, T>> bridge_exterior_and_interior(
        const std::vector<std::pair<T, T>> &exterior,
        const std::vector<std::vector<std::pair<T, T>>> &interiors) {

        if (interiors.size() == 0) {
            return exterior;
        }

        ads::DoubleLinkedList<size_t> exterior_reflex_vertices;
        for (size_t i = 0; i < exterior.size(); i++) {
            const auto prev = exterior[i + exterior.size() - 1];
            const auto curr = exterior[i];
            const auto next = exterior[i + 1];
            if (!is_convex(prev, curr, next)) {
                exterior_reflex_vertices.push_back(i);
            }
        }

        std::vector<std::pair<T, T>> bridged_vertices = exterior;
        /*std::vector<bool> is_on_exterior(bridged_vertices.size(), true);*/

        for (const auto &interior : interiors) {

            DEBUG_ASSERT(interior.size() != 0,
                         "Error: Did not expect an internal polygon with no vertices.");

            DEBUG_CODE({
                const bool exterior_winding = math::signed_area(exterior) < 0.0f;
                const bool interior_winding = math::signed_area(interior) < 0.0f;
                DEBUG_ASSERT(
                    exterior_winding != interior_winding,
                    "Error: Exterior and interior winding orders need to be different.");
            });

            // Find the interior vertex with the larges x
            const size_t interior_bridge_vertex = find_vertex_with_largest_x(interior);
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
            if ((I.first - start.first) < std::numeric_limits<float>::epsilon() &&
                (I.second - start.second) < std::numeric_limits<float>::epsilon()) {

                bridged_vertices = bridge(bridged_vertices, interior, closest_edge_id,
                                          interior_bridge_vertex);
                break;
                /*continue;*/
            }

            if ((I.first - end.first) < std::numeric_limits<float>::epsilon() &&
                (I.second - end.second) < std::numeric_limits<float>::epsilon()) {
                bridged_vertices = bridge(bridged_vertices, interior,
                                          (closest_edge_id + 1) % bridged_vertices.size(),
                                          interior_bridge_vertex);
                break;
                /*continue;*/
            }

            //  Select the vertex from the edge with the largest X
            const size_t exterior_bridge_vertex =
                start.first >= end.first
                    ? closest_edge_id
                    : (closest_edge_id + 1) % bridged_vertices.size();

            const std::pair<T, T> &P = bridged_vertices[exterior_bridge_vertex];

            // Search the reflex vertices of the outer polygon, not including P if it
            // happens to be reflex. If all of these vertices are strictly outside
            // triangle ⟨M, I, P ⟩, then M and P are mutually visible and the
            // algorithm terminates.
            const bool I_is_ear = is_ear(P, I, M, exterior, exterior_reflex_vertices);
            if (I_is_ear) {
                bridged_vertices = bridge(bridged_vertices, interior,
                                          exterior_bridge_vertex, interior_bridge_vertex);
                break;
                /*continue;*/
            }

            // TODO: Otherwise, at least one reflex vertex lies in ⟨M, I, P ⟩. Search
            // for the reflex R that minimizes the angle between ⟨M, I⟩ and ⟨M, R⟩;
            // then M and R are mutually visible and the algorithm terminates.
            const std::pair<float, float> v_mi = make_vector(M, I);
            for (const size_t &i : exterior_reflex_vertices) {
                const std::pair<float, float> R = exterior[i];
                const std::pair<float, float> v_mr = make_vector(M, R);
                DEBUG_ASSERT(false, "Not yet implemented.");
            }

            bridged_vertices = bridge(bridged_vertices, interior, exterior_bridge_vertex,
                                      interior_bridge_vertex);
            break;
        }

        return bridged_vertices;
    }

    std::vector<std::pair<T, T>> bridge(const std::vector<std::pair<T, T>> &v1,
                                        const std::vector<std::pair<T, T>> &v2,
                                        const size_t split, const size_t v2_start) {

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

    size_t find_vertex_with_largest_x(const std::vector<std::pair<T, T>> &vertices) {
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

    std::pair<size_t, std::pair<T, T>>
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
} // namespace triangulation
