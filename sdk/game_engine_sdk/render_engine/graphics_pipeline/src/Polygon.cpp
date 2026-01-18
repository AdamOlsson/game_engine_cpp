#include "graphics_pipeline/Polygon.h"
#include "math/winding.h"
#include "util/assert.h"

std::vector<graphics_pipeline::Polygon> graphics_pipeline::Polygon::construct_polygons(
    const std::vector<std::vector<std::pair<float, float>>> &outlines) {

    std::vector<size_t> nesting_levels(outlines.size());
    std::vector<std::vector<size_t>> children(outlines.size());

    for (size_t i = 0; i < outlines.size(); i++) {
        if (outlines[i].empty()) {
            continue;
        }
        const auto &inner = outlines[i];
        for (size_t j = 0; j < outlines.size(); j++) {
            if (i == j || outlines[j].empty()) {
                continue;
            }

            const auto &outer = outlines[j];
            if (outline_contains_outline(inner, outer)) {
                nesting_levels[i]++;
                children[j].push_back(i);
            }
        }
    }

    std::vector<size_t> exterior_outlines;
    exterior_outlines.reserve(outlines.size());
    for (size_t i = 0; i < nesting_levels.size(); i++) {
        if (nesting_levels[i] % 2 == 0) {
            exterior_outlines.push_back(i);
        }
    }

    DEBUG_ASSERT(exterior_outlines.size() > 0,
                 "Error: Failed to find any exterior outlines.");

    std::vector<Polygon> polygons;
    polygons.reserve(exterior_outlines.size());
    for (const size_t &exterior_idx : exterior_outlines) {
        std::vector<std::vector<std::pair<float, float>>> polygon_outlines;
        polygon_outlines.reserve(outlines.size());
        polygon_outlines.push_back(outlines[exterior_idx]);

        for (const auto &child_idx : children[exterior_idx]) {
            if (nesting_levels[exterior_idx] == nesting_levels[child_idx] - 1) {
                polygon_outlines.push_back(outlines[child_idx]);
            }
        }

        polygons.emplace_back(std::move(polygon_outlines));
    }

    return polygons;
}

std::vector<graphics_pipeline::Polygon> graphics_pipeline::Polygon::construct_polygons(
    const std::vector<std::vector<std::array<std::pair<float, float>, 3>>>
        &quadratic_curve_segments) {

    std::vector<std::vector<std::pair<float, float>>> on_curve_points(
        quadratic_curve_segments.size());

    // Extract all on curve points to do containment test
    for (size_t i = 0; i < quadratic_curve_segments.size(); i++) {
        const std::vector<std::array<std::pair<float, float>, 3>> &segment =
            quadratic_curve_segments[i];
        on_curve_points[i].reserve(segment.size());
        for (const std::array<std::pair<float, float>, 3> &curve : segment) {
            on_curve_points[i].push_back(curve[0]);
        }
        if (!segment.empty()) {
            on_curve_points[i].push_back(segment.back()[2]);
        }
    }

    std::vector<size_t> nesting_levels(on_curve_points.size());
    std::vector<std::vector<size_t>> children(on_curve_points.size());

    for (size_t i = 0; i < on_curve_points.size(); i++) {
        if (on_curve_points[i].empty()) {
            continue;
        }
        const auto &inner = on_curve_points[i];
        for (size_t j = 0; j < on_curve_points.size(); j++) {
            if (i == j || on_curve_points[j].empty()) {
                continue;
            }

            const auto &outer = on_curve_points[j];
            if (outline_contains_outline(inner, outer)) {
                nesting_levels[i]++;
                children[j].push_back(i);
            }
        }
    }

    std::vector<size_t> exterior_outlines;
    exterior_outlines.reserve(on_curve_points.size());
    for (size_t i = 0; i < nesting_levels.size(); i++) {
        if (nesting_levels[i] % 2 == 0) {
            exterior_outlines.push_back(i);
        }
    }

    DEBUG_ASSERT(exterior_outlines.size() > 0,
                 "Error: Failed to find any exterior outlines.");

    std::vector<Polygon> polygons;
    polygons.reserve(exterior_outlines.size());
    for (const size_t &exterior_idx : exterior_outlines) {
        std::vector<std::vector<std::array<std::pair<float, float>, 3>>>
            polygon_quadratic_curves;
        polygon_quadratic_curves.push_back(quadratic_curve_segments[exterior_idx]);

        for (const auto &child_idx : children[exterior_idx]) {
            if (nesting_levels[exterior_idx] == nesting_levels[child_idx] - 1) {
                polygon_quadratic_curves.push_back(quadratic_curve_segments[child_idx]);
            }
        }

        polygons.emplace_back(std::move(polygon_quadratic_curves));
    }

    return polygons;
}

std::vector<graphics_pipeline::Polygon> graphics_pipeline::Polygon::construct_polygons(
    const font::GlyphOutlines &glyph_outlines) {

    const auto &outlines = glyph_outlines.line_segments;
    const auto &curves = glyph_outlines.quadratic_curves;

    std::vector<size_t> nesting_levels(outlines.size());
    std::vector<std::vector<size_t>> children(outlines.size());

    for (size_t i = 0; i < outlines.size(); i++) {
        if (outlines[i].empty()) {
            continue;
        }
        const auto &inner = outlines[i];
        for (size_t j = 0; j < outlines.size(); j++) {
            if (i == j || outlines[j].empty()) {
                continue;
            }

            const auto &outer = outlines[j];
            if (outline_contains_outline(inner, outer)) {
                nesting_levels[i]++;
                children[j].push_back(i);
            }
        }
    }

    std::vector<size_t> exterior_outlines;
    exterior_outlines.reserve(outlines.size());
    for (size_t i = 0; i < nesting_levels.size(); i++) {
        if (nesting_levels[i] % 2 == 0) {
            exterior_outlines.push_back(i);
        }
    }

    DEBUG_ASSERT(exterior_outlines.size() > 0,
                 "Error: Failed to find any exterior outlines.");

    std::vector<Polygon> polygons;
    polygons.reserve(exterior_outlines.size());
    for (const size_t &exterior_idx : exterior_outlines) {
        std::vector<std::vector<std::pair<float, float>>> polygon_line_segments;
        std::vector<std::vector<std::array<std::pair<float, float>, 3>>>
            polygon_curve_segments;

        // Estimate the required memory
        polygon_line_segments.reserve(children[exterior_idx].size());
        polygon_curve_segments.reserve(curves.size());

        polygon_line_segments.push_back(outlines[exterior_idx]);
        if (!curves[exterior_idx].empty()) {
            polygon_curve_segments.push_back(curves[exterior_idx]);
        }

        for (const auto &child_idx : children[exterior_idx]) {
            if (nesting_levels[exterior_idx] == nesting_levels[child_idx] - 1) {
                polygon_line_segments.push_back(outlines[child_idx]);
                if (!curves[child_idx].empty()) {
                    polygon_curve_segments.push_back(curves[child_idx]);
                }
            }
        }

        polygons.emplace_back(std::move(polygon_line_segments),
                              std::move(polygon_curve_segments));
    }

    return polygons;
}

graphics_pipeline::Polygon::Polygon(
    const std::vector<std::vector<std::pair<float, float>>> &outlines)
    : m_outlines(outlines) {
    // Ensure that the exterior outline is clockwise and interior is counter clockwise
    if (m_outlines.size() > 1) {
        if (!math::is_clockwise_winding(m_outlines[0])) {
            std::ranges::reverse(m_outlines[0]);
        }

        for (size_t i = 1; i < outlines.size(); i++) {
            if (!math::is_counter_clockwise_winding(m_outlines[i])) {
                std::ranges::reverse(m_outlines[i]);
            }
        }
    }
}

graphics_pipeline::Polygon::Polygon(
    const std::vector<std::vector<std::array<std::pair<float, float>, 3>>>
        &quadratic_curves)
    : m_quadratic_curves(quadratic_curves) {}

graphics_pipeline::Polygon::Polygon(
    const std::vector<std::vector<std::pair<float, float>>> &outlines,
    const std::vector<std::vector<std::array<std::pair<float, float>, 3>>>
        &quadratic_curves)
    : m_outlines(outlines), m_quadratic_curves(quadratic_curves) {

    // Ensure that the extrior outline is clockwise and interior is counter clockwise
    if (m_outlines.size() > 1) {
        if (!math::is_clockwise_winding(m_outlines[0])) {
            std::ranges::reverse(m_outlines[0]);
        }

        for (size_t i = 1; i < outlines.size(); i++) {
            if (!math::is_counter_clockwise_winding(m_outlines[i])) {
                std::ranges::reverse(m_outlines[i]);
            }
        }
    }
}

bool graphics_pipeline::Polygon::has_consistent_winding(
    const std::vector<std::array<std::pair<float, float>, 3>> &contour) {
    if (contour.empty()) {
        return true;
    }

    const int initial_winding = int(math::is_clockwise_winding(contour[0]));
    for (size_t i = 1; i < contour.size(); i++) {
        const std::array<std::pair<float, float>, 3> &curve = contour[i];
        if (initial_winding != int(math::is_clockwise_winding(curve))) {
            return false;
        }
    }
    return true;
};

void graphics_pipeline::Polygon::reverse_contour(
    std::vector<std::array<std::pair<float, float>, 3>> &contour) {
    std::ranges::reverse(contour);

    // Also reverse each curve's points so p0->p1->p2 becomes p2->p1->p0
    for (auto &curve : contour) {
        std::swap(curve[0], curve[2]); // Swap p0 and p2, keep p1 (control point)
    }
}

const std::vector<std::pair<float, float>> &
graphics_pipeline::Polygon::get_exterior_outline() const {
    if (m_outlines.empty()) {
        throw std::runtime_error("Error: There are not outlines in polygon.");
    }
    return m_outlines[0];
}

const std::vector<std::vector<std::pair<float, float>>> &
graphics_pipeline::Polygon::get_outlines() const {
    return m_outlines;
}

const std::vector<std::vector<std::array<std::pair<float, float>, 3>>> &
graphics_pipeline::Polygon::get_quadratic_curves() const {
    return m_quadratic_curves;
}

std::span<const std::vector<std::pair<float, float>>>
graphics_pipeline::Polygon::get_holes() const {
    if (m_outlines.size() <= 1) {
        return {}; // return empty span
    }
    return std::span(m_outlines).subspan(1);
}

bool graphics_pipeline::Polygon::outline_contains_outline(
    const std::vector<std::pair<float, float>> &inner,
    const std::vector<std::pair<float, float>> &outer) {
    for (const auto &v : inner) {
        if (!math::winding_number_containment_closed_set(v, outer)) {
            return false;
        }
    }
    return true;
}
