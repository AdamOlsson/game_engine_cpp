#pragma once

#include "util/assert.h"
#include <vector>
namespace triangulation {

template <typename T> class Earcut {
  public:
    static std::vector<std::array<size_t, 3>>
    run(const std::vector<std::pair<T, T>> &exterior_vertices,
        const std::vector<std::pair<T, T>> &interior_vertices) {
        Earcut<T> ec;
        return ec.run_(exterior_vertices, interior_vertices);
    }

  private:
    std::vector<bool> m_is_reflex_vertex;
    std::vector<bool> m_is_convex_vertex;
    std::vector<bool> m_is_removed;
    std::vector<bool> m_is_ear_vertex;
    std::vector<size_t> m_next_id;
    std::vector<size_t> m_prev_id;

    Earcut() = default;
    std::vector<std::array<size_t, 3>>
    run_(const std::vector<std::pair<T, T>> &exterior_vertices,
         const std::vector<std::pair<T, T>> &interior_vertices) {

        if (exterior_vertices.size() < 3) {
            return std::vector<std::array<size_t, 3>>{};
        }

        m_is_reflex_vertex.resize(exterior_vertices.size());
        m_is_convex_vertex.resize(exterior_vertices.size());
        m_is_removed.resize(exterior_vertices.size());
        m_next_id.resize(exterior_vertices.size());
        m_prev_id.resize(exterior_vertices.size());

        // Classify exterior_vertices as convex or reflex
        const size_t exterior_vertices_size = exterior_vertices.size();
        for (auto i = 0; i < exterior_vertices.size(); i++) {
            // Populate the prev and next vectors
            m_prev_id[i] = (i + exterior_vertices_size - 1) % exterior_vertices_size;
            m_next_id[i] = (i + 1) % exterior_vertices_size;

            const bool is_conv =
                is_convex(exterior_vertices[m_prev_id[i]], exterior_vertices[i],
                          exterior_vertices[m_next_id[i]]);

            m_is_convex_vertex[i] = is_conv;
            m_is_reflex_vertex[i] = !is_conv;
            m_is_removed[i] = false;
        }

        // Determine which exterior_vertices are m_is_ear_vertex
        m_is_ear_vertex.resize(exterior_vertices.size());
        for (size_t i = 0; i < m_is_convex_vertex.size(); i++) {
            if (!m_is_convex_vertex[i]) {
                continue;
            }

            m_is_ear_vertex[i] =
                is_ear(exterior_vertices[m_prev_id[i]], exterior_vertices[i],
                       exterior_vertices[m_next_id[i]], exterior_vertices);

            DEBUG_CODE({
                if (m_is_ear_vertex[i]) {
                    DEBUG_ASSERT(m_is_convex_vertex[i],
                                 "Error: all ear vertices are ear vertices.");
                }
            });
        }

        DEBUG_ASSERT(m_is_ear_vertex.size() > 0, "Error: Found no ears in the polygon.");

        std::vector<std::array<size_t, 3>> triangles;
        triangles.reserve(exterior_vertices.size() - 2);
        size_t i = 0;
        // IMPROVEMENT: Use doubly linked list for ears and pop from front when removing
        // indices. This would save some iterations where i is not an ear.
        while (triangles.size() < exterior_vertices.size() - 2) {
            if (!m_is_ear_vertex[i] || m_is_removed[i]) {
                i = (i + 1) % exterior_vertices.size();
                continue;
            }

            triangles.emplace_back(std::array{m_prev_id[i], i, m_next_id[i]});

            if (triangles.size() >= exterior_vertices.size() - 2) {
                break;
            }

            m_next_id[m_prev_id[i]] = m_next_id[i];
            m_prev_id[m_next_id[i]] = m_prev_id[i];
            m_is_removed[i] = true;
            m_is_ear_vertex[i] = false;
            m_is_convex_vertex[i] = false;
            m_is_reflex_vertex[i] = false;

            reclassify_vertex(m_prev_id[i], exterior_vertices);
            reclassify_vertex(m_next_id[i], exterior_vertices);

            i = (i + 1) % exterior_vertices.size();
        }

        return triangles;
    }

    void reclassify_vertex(const size_t i, const std::vector<std::pair<T, T>> &vertices) {

        const auto v_prev = vertices[m_prev_id[i]];
        const auto v_curr = vertices[i];
        const auto v_next = vertices[m_next_id[i]];

        if (m_is_reflex_vertex[i]) {
            const bool is_conv = is_convex(v_prev, v_curr, v_next);
            m_is_convex_vertex[i] = is_conv;
            m_is_reflex_vertex[i] = !is_conv;
        }

        // Wether the vertex became an ear or remain an ear after the adjacent vertex was
        // removed, if it is convex we need to check if it is an ear
        if (m_is_convex_vertex[i]) {
            m_is_ear_vertex[i] = is_ear(v_prev, v_curr, v_next, vertices);
        }

        DEBUG_CODE({
            if (m_is_ear_vertex[i]) {
                DEBUG_ASSERT(m_is_convex_vertex[i],
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

        for (size_t other_vertice = 0; other_vertice < m_is_reflex_vertex.size();
             other_vertice++) {
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
};
} // namespace triangulation
