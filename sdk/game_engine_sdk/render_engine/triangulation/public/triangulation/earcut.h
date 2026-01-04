#pragma once

#include "ads/DoubleLinkedList.h"
#include "detail/earcut_impl.h"
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
            detail::EarcutImpl<T>::bridge_exterior_and_interior(exterior_vertices,
                                                                interior_vertices);

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

            const bool is_conv = detail::EarcutImpl<T>::is_convex(
                bridged_vertices[m_prev_id[i]], bridged_vertices[i],
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

            m_is_ear_vertex[i] = detail::EarcutImpl<T>::is_ear(
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

        const bool now_conv = detail::EarcutImpl<T>::is_convex(v_prev, v_curr, v_next);
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
            const bool now_ear = detail::EarcutImpl<T>::is_ear(
                v_prev, v_curr, v_next, vertices, m_reflex_vertices);
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
};
} // namespace triangulation
