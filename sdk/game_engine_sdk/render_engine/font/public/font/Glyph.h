#pragma once
#include <vector>
namespace font {

template <typename T> using Vertex = std::pair<T, T>;
template <typename T> using UVW = std::array<T, 3>;
template <typename T> using Triangle = std::array<Vertex<T>, 3>;
template <typename T> using Outline = std::vector<T>;

struct ExteriorTriangle {
    bool clockwise_winding = false;
    std::array<Vertex<float>, 3> vertices;
    std::array<UVW<float>, 3> uvw;
};

struct GlyphOutline {
    std::vector<font::Vertex<float>> vertices;
    std::vector<font::ExteriorTriangle> curves;
};

using GlyphOutlineCollection = std::vector<GlyphOutline>;

// A polygon can only have one exterior outline, but multiple interior
// outlines (holes)
struct Polygon {
    Outline<Vertex<float>> exterior_outline;
    std::vector<Outline<Vertex<float>>> interior_outlines;
    std::vector<ExteriorTriangle> curves;
};

struct Glyph {
    std::string name;
    std::vector<Polygon> polygons;

    static std::vector<font::Polygon>
    construct_polygons(std::vector<GlyphOutline> &&glyph_outlines);

    static bool
    is_point_inside_outline(const std::pair<float, float> &p,
                            const std::vector<std::pair<float, float>> &outline);
};

} // namespace font
