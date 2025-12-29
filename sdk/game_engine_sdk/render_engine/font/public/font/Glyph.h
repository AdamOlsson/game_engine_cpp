#pragma once
#include <vector>
namespace font {

template <typename T> using Vertex = std::pair<T, T>;
template <typename T> using UVW = std::array<T, 3>;
template <typename T> using Triangle = std::array<Vertex<T>, 3>;

struct ExteriorTriangle {
    std::array<Vertex<float>, 3> vertices;
    std::array<UVW<float>, 3> uvw;
};

struct GlyphVertices {
    std::vector<font::Vertex<float>> interior;
    std::vector<font::ExteriorTriangle> exterior;
};

using GlyphVertexCollection = std::vector<GlyphVertices>;

struct Glyph {
    std::string name;
    GlyphVertexCollection vertices;
};

} // namespace font
