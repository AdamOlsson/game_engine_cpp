#pragma once

#include <array>
#include <vector>

namespace math {
template <typename T>
float signed_area_triangle(const std::array<std::pair<T, T>, 3> &vertices) {
    float area = 0.0f;
    for (size_t i = 0; i < vertices.size(); i++) {
        size_t j = (i + 1) % vertices.size();
        area += (vertices[j].first - vertices[i].first) *
                (vertices[j].second + vertices[i].second);
    }
    return area * 0.5f;
}

template <typename T> float signed_area(const std::vector<std::pair<T, T>> &vertices) {
    float area = 0.0f;
    for (size_t i = 0; i < vertices.size(); i++) {
        size_t j = (i + 1) % vertices.size();
        area += (vertices[j].first - vertices[i].first) *
                (vertices[j].second + vertices[i].second);
    }
    return area * 0.5f;
}
} // namespace math
