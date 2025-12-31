#pragma once

#include "font/Glyph.h"
#include "math/area.h"
namespace font {

constexpr bool is_clockwise_winding(const font::Triangle<float> &triangle) {
    return math::signed_area_triangle(triangle) > 0.0f;
}

constexpr bool is_clockwise_winding(const std::vector<Vertex<float>> &polygon) {
    return math::signed_area(polygon) > 0.0f;
}

}; // namespace font
