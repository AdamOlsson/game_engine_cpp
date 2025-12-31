#pragma once
#include <vector>
namespace font {

template <typename T> using Vertex = std::pair<T, T>;
template <typename T> using UVW = std::array<T, 3>;
template <typename T> using Triangle = std::array<Vertex<T>, 3>;

struct ExteriorTriangle {
    bool clockwise_winding = false;
    std::array<Vertex<float>, 3> vertices;
    std::array<UVW<float>, 3> uvw;
};

struct GlyphOutline {
    std::vector<font::Vertex<float>> vertices;
    std::vector<font::ExteriorTriangle> curves;
};

using GlyphVertexCollection = std::vector<GlyphOutline>;
using GlyphOutlineCollection = std::vector<GlyphOutline>;

// A polygon can only have one exterior outline, but multiple interior
// outlines (holes)
template <typename T> using Outline = std::vector<T>;
struct Polygon {
    Outline<Vertex<float>> exterior_outline;
    std::vector<Outline<Vertex<float>>> interior_outlines;
};

struct Glyph {
    std::string name;
    std::vector<Polygon> polygons;
    std::vector<ExteriorTriangle> curves;
};

} // namespace font
