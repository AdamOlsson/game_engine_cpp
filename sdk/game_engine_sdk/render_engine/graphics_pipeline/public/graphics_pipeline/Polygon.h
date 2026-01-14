#pragma once

#include <span>
#include <vector>
namespace graphics_pipeline {

class Polygon {
  public:
    Polygon(const std::vector<std::vector<std::pair<float, float>>> &outlines);

    static std::vector<Polygon>
    construct_polygons(const std::vector<std::vector<std::pair<float, float>>> &outlines);

    const std::vector<std::vector<std::pair<float, float>>> &get_outlines() const;
    const std::vector<std::pair<float, float>> &get_exterior_outline() const;
    std::span<const std::vector<std::pair<float, float>>> get_holes() const;

  private:
    std::vector<std::vector<std::pair<float, float>>> m_outlines;

    static bool
    outline_contains_outline(const std::vector<std::pair<float, float>> &inner,
                             const std::vector<std::pair<float, float>> &outer);
};

} // namespace graphics_pipeline
