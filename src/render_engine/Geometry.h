#pragma once

#include "render_engine/shapes/Vertex.h"
#include <vector>
inline std::vector<Vertex> generate_circle_vertices(int num_points) {
    std::vector<Vertex> vertices;
    vertices.reserve(num_points + 1);

    vertices.push_back({0.0f, 0.0f, 0.0f});

    const float radius = 0.5f;
    const float angle_increment = -2.0f * M_PI / num_points;

    for (int i = 0; i < num_points; ++i) {
        const float angle = i * angle_increment;
        const float x = radius * std::cos(angle);
        const float y = radius * std::sin(angle);

        vertices.push_back({x, y, 0.0f});
    }

    return vertices;
}

inline std::vector<uint16_t> generate_circle_indices(int num_points) {
    std::vector<uint16_t> indices;
    indices.reserve(3 * (num_points - 1));

    for (uint16_t i = 1; i < num_points - 1; ++i) {
        indices.push_back(0);
        indices.push_back(i);
        indices.push_back(i + 1);
    }
    indices.push_back(0);
    indices.push_back(num_points - 1);
    indices.push_back(1);

    return indices;
}

namespace Geometry {

const std::vector<uint16_t> circle_indices = generate_circle_indices(180);
const std::vector<Vertex> circle_vertices = generate_circle_vertices(180);

const std::vector<uint16_t> triangle_indices = {0, 1, 2};
const std::vector<Vertex> triangle_vertices = {
    {{0.0f, -0.577f, 0.0f}, {-0.5f, 0.289f, 0.0f}, {0.5f, 0.289f, 0.0f}}};

const std::vector<uint16_t> rectangle_indices = {0, 1, 2, 0, 2, 3};
const std::vector<Vertex> rectangle_vertices = {
    {-0.5f, -0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f}, {0.5f, 0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}};

const std::vector<uint16_t> hexagon_indices = {0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5};
const std::vector<Vertex> hexagon_vertices = {
    {-0.577350f, 0.0f, 0.0f}, {-0.28865, 0.5, 0.0f},   {0.28865f, 0.5, 0.0f},
    {0.577350f, 0.0f, 0.0f},  {0.28865f, -0.5f, 0.0f}, {-0.28865f, -0.5, 0.0f},
};

} // namespace Geometry
