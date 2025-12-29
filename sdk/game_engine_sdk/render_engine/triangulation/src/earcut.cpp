#include "triangulation/earcut.h"

std::pair<int, int> triangulation::make_vector(const std::pair<int, int> &from,
                                               const std::pair<int, int> &to) {
    return std::make_pair<int, int>(to.first - from.first, to.second - from.second);
}

bool triangulation::is_convex(const std::pair<int, int> &v_prev,
                              const std::pair<int, int> &v_curr,
                              const std::pair<int, int> &v_next) {
    const auto va = make_vector(v_curr, v_prev);
    const auto vb = make_vector(v_curr, v_next);
    const int cross = triangulation::cross_prod(va, vb);
    return cross < 0;
}

int triangulation::cross_prod(const std::pair<int, int> &va,
                              const std::pair<int, int> &vb) {
    return va.first * vb.second - va.second * vb.first;
}

bool triangulation::is_ear(const std::pair<int, int> &v_prev,
                           const std::pair<int, int> &v_curr,
                           const std::pair<int, int> &v_next,
                           const std::vector<std::pair<int, int>> &vertices,
                           const std::vector<bool> &is_reflex_vertex) {

    // Create edges pointing in a counter clockwise order
    const auto vab = make_vector(v_prev, v_curr);
    const auto vbc = make_vector(v_curr, v_next);
    const auto vca = make_vector(v_next, v_prev);

    for (size_t other_vertice = 0; other_vertice < is_reflex_vertex.size();
         other_vertice++) {
        const auto &p = vertices[other_vertice];

        if (!is_reflex_vertex[other_vertice] || p == v_prev || p == v_curr ||
            p == v_next) {
            continue;
        }

        const auto vpa = make_vector(p, v_prev);
        const auto vpb = make_vector(p, v_curr);
        const auto vpc = make_vector(p, v_next);

        // Containment test
        const int cross_a = cross_prod(vab, vpa);
        const int cross_b = cross_prod(vbc, vpb);
        const int cross_c = cross_prod(vca, vpc);
        if (cross_a <= 0 && cross_b <= 0 && cross_c <= 0) {
            // point p is inside the triangle
            return false;
        }
    }
    return true;
}

void triangulation::reclassify_vertex(const size_t i,
                                      const std::vector<std::pair<int, int>> &vertices,
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
