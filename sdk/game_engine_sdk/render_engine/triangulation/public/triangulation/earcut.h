#pragma once

#include "util/assert.h"
#include <vector>
namespace triangulation {

template <typename T> using Vertex = std::pair<T, T>;
using Index = size_t;

template <typename T>
std::pair<T, T> make_vector(const std::pair<T, T> &from, const std::pair<T, T> &to) {
    return std::make_pair<int, int>(to.first - from.first, to.second - from.second);
}

template <typename T> T cross_prod(const std::pair<T, T> &va, const std::pair<T, T> &vb) {
    return va.first * vb.second - va.second * vb.first;
}

template <typename T>
bool is_convex(const std::pair<T, T> &v_prev, const std::pair<T, T> &v_curr,
               const std::pair<T, T> &v_next) {
    const auto va = make_vector(v_curr, v_prev);
    const auto vb = make_vector(v_curr, v_next);
    const auto cross = triangulation::cross_prod(va, vb);
    return cross < 0;
}

bool is_contained(const std::pair<int, int> &p, const std::pair<int, int> &va,
                  const std::pair<int, int> &vb, const std::pair<int, int> &vc);

template <typename T>
bool is_ear(const std::pair<T, T> &v_prev, const std::pair<T, T> &v_curr,
            const std::pair<T, T> &v_next, const std::vector<std::pair<T, T>> &vertices,
            const std::vector<bool> &is_reflex_vertex) {

    // Create edges pointing in a counter clockwise order
    const std::pair<T, T> vab = make_vector(v_prev, v_curr);
    const std::pair<T, T> vbc = make_vector(v_curr, v_next);
    const std::pair<T, T> vca = make_vector(v_next, v_prev);

    for (size_t other_vertice = 0; other_vertice < is_reflex_vertex.size();
         other_vertice++) {
        const auto &p = vertices[other_vertice];

        if (!is_reflex_vertex[other_vertice] || p == v_prev || p == v_curr ||
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
        if (cross_a <= 0 && cross_b <= 0 && cross_c <= 0) {
            // point p is inside the triangle
            return false;
        }
    }
    return true;
}

template <typename T>
void reclassify_vertex(const size_t i, const std::vector<std::pair<T, T>> &vertices,
                       const std::vector<size_t> &prev_id,
                       const std::vector<size_t> &next_id,
                       std::vector<bool> &is_reflex_vertex,
                       std::vector<bool> &is_convex_vertex,
                       std::vector<bool> &is_ear_vertex) {

    const auto v_prev = vertices[prev_id[i]];
    const auto v_curr = vertices[i];
    const auto v_next = vertices[next_id[i]];

    if (is_reflex_vertex[i]) {
        const bool is_conv = is_convex(v_prev, v_curr, v_next);
        is_convex_vertex[i] = is_conv;
        is_reflex_vertex[i] = !is_conv;
    }

    // Wether the vertex became an ear or remain an ear after the adjacent vertex was
    // removed, if it is convex we need to check if it is an ear
    if (is_convex_vertex[i]) {
        is_ear_vertex[i] = is_ear(v_prev, v_curr, v_next, vertices, is_reflex_vertex);
    }

    DEBUG_CODE({
        if (is_ear_vertex[i]) {
            DEBUG_ASSERT(is_convex_vertex[i],
                         "Error: all ear vertices are ear vertices.");
        }
    });
}

template <typename T>
concept IndexOrVertex = std::is_same_v<T, Index> || std::is_same_v<T, Vertex<float>> ||
                        std::is_same_v<T, Vertex<int>>;

template <typename T = int, IndexOrVertex ReturnType = Index>
std::vector<std::array<ReturnType, 3>>
earcut(const std::vector<std::pair<T, T>> &vertices) {
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
