#pragma once

#include <span>
#include <vector>
namespace graphics_pipeline {

class Polygon {
  public:
    Polygon(const std::vector<std::vector<std::pair<float, float>>> &outlines);
    Polygon(const std::vector<std::vector<std::array<std::pair<float, float>, 3>>>
                &quadratic_curves);

    static std::vector<Polygon>
    construct_polygons(const std::vector<std::vector<std::pair<float, float>>> &outlines);

    static std::vector<Polygon> construct_polygons(
        const std::vector<std::vector<std::array<std::pair<float, float>, 3>>>
            &quadratic_curves);

    const std::vector<std::vector<std::pair<float, float>>> &get_outlines() const;
    const std::vector<std::pair<float, float>> &get_exterior_outline() const;
    std::span<const std::vector<std::pair<float, float>>> get_holes() const;

    const std::vector<std::vector<std::array<std::pair<float, float>, 3>>> &
    get_quadratic_curves() const;

  private:
    std::vector<std::vector<std::pair<float, float>>> m_outlines;
    std::vector<std::vector<std::array<std::pair<float, float>, 3>>> m_quadratic_curves;

    static bool
    outline_contains_outline(const std::vector<std::pair<float, float>> &inner,
                             const std::vector<std::pair<float, float>> &outer);

    bool has_consistent_winding(
        const std::vector<std::array<std::pair<float, float>, 3>> &contour);

    void reverse_contour(std::vector<std::array<std::pair<float, float>, 3>> &contour);
};
} // namespace graphics_pipeline
