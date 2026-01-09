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
point_on_segment(const font::Vertex<float> &point, const font::Vertex<float> &p1,
                 const font::Vertex<float> &p2,
                 double epsilon = std::numeric_limits<float>::epsilon() * 100) {
    // Check if point is collinear with segment
    double cross = (p2.first - p1.first) * (point.second - p1.second) -
                   (point.first - p1.first) * (p2.second - p1.second);

    if (std::abs(cross) > epsilon) {
        return false; // Not collinear
    }

    // Check if point is between p1 and p2
    double minX = std::min(p1.first, p2.first) - epsilon;
    double maxX = std::max(p1.first, p2.first) + epsilon;
    double minY = std::min(p1.second, p2.second) - epsilon;
    double maxY = std::max(p1.second, p2.second) + epsilon;

    return point.first >= minX && point.first <= maxX && point.second >= minY &&
           point.second <= maxY;
}

constexpr std::pair<font::Vertex<float>, font::Vertex<float>>
find_bounding_box(const std::vector<Vertex<float>> &outline) {
    if (outline.empty()) {
        return {{0.0f, 0.0f}, {0.0f, 0.0f}};
    }

    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float max_y = std::numeric_limits<float>::lowest();

    for (const auto &[x, y] : outline) {
        min_x = std::min(min_x, x);
        min_y = std::min(min_y, y);
        max_x = std::max(max_x, x);
        max_y = std::max(max_y, y);
    }

    return {{min_x, min_y}, {max_x, max_y}};
}
constexpr bool
winding_number_containment_open_set(const font::Vertex<float> &p,
                                    const std::vector<font::Vertex<float>> &outline) {
    if (outline.size() < 3) {
        return false;
    }

    DEBUG_ASSERT(is_counter_clockwise_winding(outline),
                 "Error: Expect the outline to be counter clockwise for the winding "
                 "number containment test to work.");

    const std::pair<Vertex<float>, Vertex<float>> bbox = find_bounding_box(outline);
    const Vertex<float> bot_left = bbox.first;
    const Vertex<float> top_right = bbox.second;
    if (p.first < bot_left.first || top_right.first < p.first ||
        p.second < bot_left.second || top_right.second < p.second) {
        return false;
    }

    // First check if point is on any edge (boundary = not in open set)
    for (size_t i = 0; i < outline.size(); i++) {
        const font::Vertex<float> &start = outline[i];
        const font::Vertex<float> &end = outline[(i + 1) % outline.size()];

        if (point_on_segment(p, start, end,
                             std::numeric_limits<float>::epsilon() * 100)) {
            return false; // On boundary  P
        }
    }

    int winding = 0;

    for (size_t i = 0; i < outline.size(); i++) {
        const std::pair<float, float> &start = outline[i];
        const std::pair<float, float> &end = outline[(i + 1) % outline.size()];

        if (start.second <= p.second) {
            if (end.second > p.second) {
                // Upward crossing
                float cross = (end.first - start.first) * (p.second - start.second) -
                              (p.first - start.first) * (end.second - p.second);

                if (cross > 0.0f) {
                    winding++;
                }
            }
        } else {
            if (end.second <= p.second) {
                // Downrward crossing
                float cross = (end.first - start.first) * (p.second - start.second) -
                              (p.first - start.first) * (end.second - p.second);

                if (cross < 0.0f) {
                    winding--;
                }
            }
        }
    }

    return winding != 0;
}

constexpr bool
winding_number_containment_closed_set(const font::Vertex<float> &p,
                                      const std::vector<font::Vertex<float>> &outline) {
    if (outline.size() < 3) {
        return false;
    }

    DEBUG_ASSERT(is_counter_clockwise_winding(outline),
                 "Error: Expect the outline to be counter clockwise for the winding "
                 "number containment test to work.");

    const std::pair<Vertex<float>, Vertex<float>> bbox = find_bounding_box(outline);
    const Vertex<float> bot_left = bbox.first;
    const Vertex<float> top_right = bbox.first;
    if (p.first < bot_left.first || top_right.first < p.first ||
        p.second < bot_left.second || top_right.second < p.second) {
        return false;
    }

    int winding = 0;

    for (size_t i = 0; i < outline.size(); i++) {
        const std::pair<float, float> &start = outline[i];
        const std::pair<float, float> &end = outline[(i + 1) % outline.size()];

        if (start.second <= p.second) {
            if (end.second > p.second) {
                // Upward crossing
                float cross = (end.first - start.first) * (p.second - start.second) -
                              (p.first - start.first) * (end.second - p.second);

                if (cross > 0.0f) {
                    winding++;
                }
            }
        } else {
            if (end.second <= p.second) {
                // Downrward crossing
                float cross = (end.first - start.first) * (p.second - start.second) -
                              (p.first - start.first) * (end.second - p.second);

                if (cross < 0.0f) {
                    winding--;
                }
            }
        }
    }

    return winding != 0;
}

}; // namespace font
