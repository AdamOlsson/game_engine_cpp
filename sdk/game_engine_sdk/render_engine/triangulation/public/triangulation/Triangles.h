#pragma once

#include <utility>
#include <vector>
namespace triangulation {

template <typename T> using Vertex = std::pair<T, T>;
using Index = size_t;
using Triangle = std::array<Index, 3>;

template <typename T> struct Triangles {
    std::vector<Vertex<T>> vertices;
    std::vector<Triangle> indices;
};

} // namespace triangulation
