#include "graphics_pipeline/Polygon.h"
#include "graphics_pipeline/winding.h"
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

graphics_pipeline::Polygon::Polygon(
    const std::vector<std::vector<std::pair<float, float>>> &outlines)
    : m_outlines(outlines) {
    // Ensure that the exterior outline is clockwise and interior is counter clockwise
    if (m_outlines.size() > 1) {
        if (!is_clockwise_winding(m_outlines[0])) {
            std::ranges::reverse(m_outlines[0]);
        }

        for (size_t i = 1; i < outlines.size(); i++) {
            if (!is_counter_clockwise_winding(m_outlines[i])) {
                std::ranges::reverse(m_outlines[i]);
            }
        }
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
        if (!winding_number_containment_closed_set(v, outer)) {
            return false;
        }
    }
    return true;
}
