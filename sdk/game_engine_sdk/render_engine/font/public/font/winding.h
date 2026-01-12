#pragma once

#include "font/Glyph.h"
#include "math/area.h"
#include "util/assert.h"
namespace font {

constexpr bool is_clockwise_winding(const font::Triangle<float> &triangle) {
    return math::signed_area_triangle(triangle) > 0.0f;
}

constexpr bool is_clockwise_winding(const float area) { return area > 0.0f; }

constexpr bool is_clockwise_winding(const std::vector<Vertex<float>> &polygon) {
    return math::signed_area(polygon) > 0.0f;
}

constexpr bool is_counter_clockwise_winding(const float area) {
    return !is_clockwise_winding(area);
}

constexpr bool is_counter_clockwise_winding(const std::vector<Vertex<float>> &polygon) {
    return !is_clockwise_winding(polygon);
}

constexpr bool
winding_number_containment_open_set(const std::pair<float, float> &p,
                                    const std::vector<std::pair<float, float>> &outline) {

    if (outline.size() < 3)
        return false;

    const float EPSILON = 1e-9f;
    int winding = 0;

    for (size_t i = 0; i < outline.size(); i++) {
        const auto &v1 = outline[i];
        const auto &v2 = outline[(i + 1) % outline.size()];

        // Check if point is exactly on this edge (for open set, return false)
        // Use parametric form: point on segment if p = v1 + t*(v2-v1) for t in [0,1]
        float dx = v2.first - v1.first;
        float dy = v2.second - v1.second;
        float len_sq = dx * dx + dy * dy;

        if (len_sq > EPSILON) {
            float t = ((p.first - v1.first) * dx + (p.second - v1.second) * dy) / len_sq;
            if (t >= 0.0f && t <= 1.0f) {
                float dist_x = p.first - (v1.first + t * dx);
                float dist_y = p.second - (v1.second + t * dy);
                float dist_sq = dist_x * dist_x + dist_y * dist_y;
                if (dist_sq < EPSILON) {
                    return false; // Point is on the boundary
                }
            }
        }

        // Standard winding number calculation
        if (v1.second <= p.second) {
            if (v2.second > p.second) {
                // Upward crossing
                float cross = (v2.first - v1.first) * (p.second - v1.second) -
                              (p.first - v1.first) * (v2.second - v1.second);
                if (cross > EPSILON) {
                    winding++;
                }
            }
        } else {
            if (v2.second <= p.second) {
                // Downward crossing
                float cross = (v2.first - v1.first) * (p.second - v1.second) -
                              (p.first - v1.first) * (v2.second - v1.second);
                if (cross < -EPSILON) {
                    winding--;
                }
            }
        }
    }

    return winding != 0;
}

}; // namespace font
