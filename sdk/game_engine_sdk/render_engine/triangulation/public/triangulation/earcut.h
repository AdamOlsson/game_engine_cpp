#pragma once

#include "util/assert.h"
#include <vector>
namespace triangulation {

using Vertex = std::pair<int, int>;
using Index = size_t;

bool is_convex(const std::pair<int, int> &v_prev, const std::pair<int, int> &v_curr,
               const std::pair<int, int> &v_next);

bool is_contained(const std::pair<int, int> &p, const std::pair<int, int> &va,
                  const std::pair<int, int> &vb, const std::pair<int, int> &vc);

bool is_ear(const std::pair<int, int> &v_prev, const std::pair<int, int> &v_curr,
            const std::pair<int, int> &v_next,
            const std::vector<std::pair<int, int>> &vertices,
            const std::vector<bool> &is_reflex_vertex);

void reclassify_vertex(const size_t i, const std::vector<std::pair<int, int>> &vertices,
                       const std::vector<size_t> &prev_id,
                       const std::vector<size_t> &next_id,
                       std::vector<bool> &is_reflex_vertex,
                       std::vector<bool> &is_convex_vertex,
                       std::vector<bool> &is_ear_vertex);

int cross_prod(const std::pair<int, int> &va, const std::pair<int, int> &vb);

std::pair<int, int> make_vector(const std::pair<int, int> &from,
                                const std::pair<int, int> &to);

template <typename T>
concept IndexOrVertex = std::is_same_v<T, Index> || std::is_same_v<T, Vertex>;

template <IndexOrVertex ReturnType = Index>
auto earcut(const std::vector<std::pair<int, int>> &vertices) {
    if (vertices.size() < 3) {
        return std::vector<std::array<ReturnType, 3>>{};
    }

    std::vector<bool> is_reflex_vertex;
    is_reflex_vertex.resize(vertices.size());

    std::vector<bool> is_convex_vertex;
    is_convex_vertex.resize(vertices.size());

    std::vector<bool> is_removed;
    is_removed.resize(vertices.size());

    std::vector<size_t> next_id;
    next_id.resize(vertices.size());

    std::vector<size_t> prev_id;
    prev_id.resize(vertices.size());

    // Classify vertices as convex or reflex
    const size_t vertices_size = vertices.size();
    for (auto i = 0; i < vertices.size(); i++) {
        // Populate the prev and next vectors
        prev_id[i] = (i + vertices_size - 1) % vertices_size;
        next_id[i] = (i + 1) % vertices_size;

        const bool is_conv =
            is_convex(vertices[prev_id[i]], vertices[i], vertices[next_id[i]]);

        is_convex_vertex[i] = is_conv;
        is_reflex_vertex[i] = !is_conv;
        is_removed[i] = false;
    }

    // Determine which vertices are is_ear_vertex
    std::vector<bool> is_ear_vertex;
    is_ear_vertex.resize(vertices.size());
    for (size_t i = 0; i < is_convex_vertex.size(); i++) {
        if (!is_convex_vertex[i]) {
            continue;
        }

        is_ear_vertex[i] = is_ear(vertices[prev_id[i]], vertices[i], vertices[next_id[i]],
                                  vertices, is_reflex_vertex);

        DEBUG_CODE({
            if (is_ear_vertex[i]) {
                DEBUG_ASSERT(is_convex_vertex[i],
                             "Error: all ear vertices are ear vertices.");
            }
        });
    }

    DEBUG_ASSERT(is_ear_vertex.size() > 0, "Error: Found no ears in the polygon.");

    std::vector<std::array<size_t, 3>> triangles;
    triangles.reserve(vertices.size() - 2);
    size_t i = 0;
    // IMPROVEMENT: Use doubly linked list for ears and pop from front when removing
    // indices. This would save some iterations where i is not an ear.
    while (triangles.size() < vertices.size() - 2) {
        if (!is_ear_vertex[i] || is_removed[i]) {
            i = (i + 1) % vertices.size();
            continue;
        }

        triangles.emplace_back(std::array{prev_id[i], i, next_id[i]});

        if (triangles.size() >= vertices.size() - 2) {
            break;
        }

        next_id[prev_id[i]] = next_id[i];
        prev_id[next_id[i]] = prev_id[i];
        is_removed[i] = true;
        is_ear_vertex[i] = false;
        is_convex_vertex[i] = false;
        is_reflex_vertex[i] = false;

        reclassify_vertex(prev_id[i], vertices, prev_id, next_id, is_reflex_vertex,
                          is_convex_vertex, is_ear_vertex);

        reclassify_vertex(next_id[i], vertices, prev_id, next_id, is_reflex_vertex,
                          is_convex_vertex, is_ear_vertex);

        i = (i + 1) % vertices.size();
    }

    return triangles;
}

} // namespace triangulation
