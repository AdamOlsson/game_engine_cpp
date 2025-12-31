#pragma once

#include "ads/DoubleLinkedList.h"
#include "math/area.h"
#include "util/assert.h"
#include <vector>
namespace triangulation {

template <typename T> class Earcut {
  public:
    static std::vector<std::array<size_t, 3>>
    run(const std::vector<std::pair<T, T>> &bridged_vertices,
        const std::vector<std::pair<T, T>> &interior_vertices) {
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
    std::vector<std::array<size_t, 3>>
    run_(const std::vector<std::pair<T, T>> &exterior_vertices,
         const std::vector<std::pair<T, T>> &interior_vertices) {

        if (exterior_vertices.size() < 3) {
            return std::vector<std::array<size_t, 3>>{};
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

            m_is_ear_vertex[i] =
                is_ear(bridged_vertices[m_prev_id[i]], bridged_vertices[i],
                       bridged_vertices[m_next_id[i]], bridged_vertices);
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

        return triangles;
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
            const bool now_ear = is_ear(v_prev, v_curr, v_next, vertices);
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
                const std::vector<std::pair<T, T>> &vertices) {

        // Create edges pointing in a counter clockwise order
        const std::pair<T, T> vab = make_vector(v_prev, v_curr);
        const std::pair<T, T> vbc = make_vector(v_curr, v_next);
        const std::pair<T, T> vca = make_vector(v_next, v_prev);

        for (const size_t &other_vertice : m_reflex_vertices) {
            const auto &p = vertices[other_vertice];

            if (!m_is_reflex_vertex[other_vertice] || p == v_prev || p == v_curr ||
                p == v_next) {
                continue;
            }

            const std::pair<T, T> vpa = make_vector(p, v_prev);
            const std::pair<T, T> vpb = make_vector(p, v_curr);
            const std::pair<T, T> vpc = make_vector(p, v_next);

            // Containment test
            const T cross_a = cross_prod(vab, vpa);
            const T cross_b = cross_prod(vbc, vpb);
            const T cross_c = cross_prod(vca, vpc);
            if (cross_a < 0 && cross_b < 0 && cross_c < 0) {
                // point p is inside the triangle
                return false;
            }
        }
        return true;
    }

    bool is_convex(const std::pair<T, T> &v_prev, const std::pair<T, T> &v_curr,
                   const std::pair<T, T> &v_next) {
        const auto va = make_vector(v_curr, v_prev);
        const auto vb = make_vector(v_curr, v_next);
        const auto cross = cross_prod(va, vb);
        return cross < 0;
    }

    std::pair<T, T> make_vector(const std::pair<T, T> &from, const std::pair<T, T> &to) {
        return std::make_pair<int, int>(to.first - from.first, to.second - from.second);
    }

    T cross_prod(const std::pair<T, T> &va, const std::pair<T, T> &vb) {
        return va.first * vb.second - va.second * vb.first;
    }

    std::vector<std::pair<T, T>>
    bridge_exterior_and_interior(const std::vector<std::pair<T, T>> &exterior,
                                 const std::vector<std::pair<T, T>> &interior) {

        if (interior.size() == 0) {
            return exterior;
        }

        DEBUG_CODE({
            const bool exterior_winding = math::signed_area(exterior) < 0.0f;
            const bool interior_winding = math::signed_area(interior) < 0.0f;
            DEBUG_ASSERT(
                exterior_winding != interior_winding,
                "Error: Exterior and interior winding orders need to be different.");
        });

        // Find the interior vertex with the larges x
        size_t interior_bridge_vertex = 0;
        T max_interior_x_found = interior[0].first;
        for (size_t i = 1; i < interior.size(); i++) {
            if (interior[i].first > max_interior_x_found) {
                max_interior_x_found = interior[i].first;
                interior_bridge_vertex = i;
            }
        }
        const std::pair<T, T> &M = interior[interior_bridge_vertex];

        float min_x = 999.0f;
        size_t closest_edge_id = exterior.size();
        for (size_t i = 0; i < exterior.size(); i++) {
            const auto &start = exterior[i];
            const auto &end = exterior[(i + 1) % exterior.size()];

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
            }
        }

        DEBUG_ASSERT(closest_edge_id < exterior.size(),
                     "Error: Failed to find intersection edge.");

        //  Select the vertex from the edge with the largest X
        const auto &start = exterior[closest_edge_id];
        const auto &end = exterior[(closest_edge_id + 1) % exterior.size()];
        const size_t exterior_bridge_vertice =
            start.first >= end.first ? closest_edge_id
                                     : (closest_edge_id + 1) % exterior.size();

        // TODO: If I is a vertex of the outer polygon, then M and I are mutually visible
        // and the algorithm terminates
        /*if (min_t_edge_id < exterior.size()) {}*/

        // TODO: Search the reflex vertices of the outer polygon, not including P if it
        // happens to be reflex. If all of these vertices are strictly outside triangle
        // ⟨M, I, P ⟩, then M and P are mutually visible and the algorithm terminates.

        // TODO:  Otherwise, at least one reflex vertex lies in ⟨M, I, P ⟩. Search for the
        // reflex R that minimizes the angle between ⟨M, I⟩ and ⟨M, R⟩; then M and R are
        // mutually visible and the algorithm terminates.

        std::vector<std::pair<T, T>> merged_vertices;
        merged_vertices.reserve(exterior.size() + interior.size());

        for (size_t i = 0; i < exterior.size(); i++) {
            merged_vertices.push_back(exterior[i]);
            if (i == exterior_bridge_vertice) {
                for (size_t j = 0; j < interior.size(); j++) {
                    merged_vertices.push_back(
                        interior[(j + interior_bridge_vertex) % interior.size()]);
                }
                // We need to start and end on the interior bridge
                merged_vertices.push_back(interior[interior_bridge_vertex]);
            }
        }

        return merged_vertices;
    }
};
} // namespace triangulation
