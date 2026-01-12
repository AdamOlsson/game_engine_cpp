#include "font/Glyph.h"
#include "font/winding.h"
#include "math/area.h"
#include <functional>

struct OutlineNode {
    size_t index = 0;
    size_t nesting_level = 0;
    std::vector<size_t> children;
};

bool outline_contains_outline(const std::vector<font::Vertex<float>> &outer,
                              const std::vector<font::Vertex<float>> &inner) {
    if (inner.empty()) {
        return false;
    }

    for (const auto &v : inner) {
        const bool is_contained = font::winding_number_containment_open_set(v, outer);
        if (!is_contained) {
            return false;
        }
    }

    return true;
}

std::vector<OutlineNode>
build_containment_tree(const std::vector<font::GlyphOutline> &glyph_outlines) {

    std::vector<OutlineNode> nodes(glyph_outlines.size());

    for (size_t i = 0; i < glyph_outlines.size(); i++) {
        nodes[i].index = i;
    }

    // In the first pass we determine the nesting level for each outline, i.e how many
    // outlines contain this outline
    std::vector<std::vector<size_t>> containers(glyph_outlines.size());
    for (size_t i = 0; i < glyph_outlines.size(); i++) {
        const auto &child = glyph_outlines[i].vertices;
        if (child.empty()) {
            continue;
        }

        for (size_t j = 0; j < glyph_outlines.size(); j++) {
            const auto &parent = glyph_outlines[j].vertices;

            if (i == j || parent.empty()) {
                continue;
            }

            if (outline_contains_outline(parent, child)) {
                containers[i].push_back(j);
            }
        }
        nodes[i].nesting_level = containers[i].size();
    }

    for (size_t i = 0; i < glyph_outlines.size(); i++) {
        const std::vector<size_t> parents = containers[i];
        if (parents.empty()) {
            continue;
        }

        // Find the closest parent, based on nesting level
        size_t closest_parent = parents[0];
        for (size_t j = 1; j < parents.size(); j++) {
            const size_t parent_id = parents[j];
            if (nodes[parent_id].nesting_level > nodes[closest_parent].nesting_level) {
                closest_parent = parent_id;
            }
        }

        nodes[closest_parent].children.push_back(i);
    }

    return nodes;
}

std::vector<font::Polygon>
font::Glyph::construct_polygons(std::vector<font::GlyphOutline> &&glyph_outlines) {

    if (glyph_outlines.empty()) {
        return {};
    }

    for (auto &outline : glyph_outlines) {
        if (font::is_clockwise_winding(outline.vertices)) {
            std::ranges::reverse(outline.vertices);
        }
    }

    const std::vector<OutlineNode> nodes = build_containment_tree(glyph_outlines);

    // Collect the outer most outlines
    std::vector<size_t> exterior_outline_ids;
    for (const auto &node : nodes) {
        if (node.nesting_level == 0 && !glyph_outlines[node.index].vertices.empty()) {
            exterior_outline_ids.push_back(node.index);
        }
    }

    std::vector<font::Polygon> polygons;
    polygons.reserve(exterior_outline_ids.size());

    for (const size_t exterior_outline_id : exterior_outline_ids) {
        font::Polygon polygon;

        const float area =
            math::signed_area(glyph_outlines[exterior_outline_id].vertices);

        if (abs(area) < std::numeric_limits<float>::epsilon()) {
            // if area is basically 0
            continue;
        } else if (font::is_clockwise_winding(area)) {
            std::ranges::reverse(glyph_outlines[exterior_outline_id].vertices);
        }

        polygon.exterior_outline =
            std::move(glyph_outlines[exterior_outline_id].vertices);

        polygon.curves.insert(polygon.curves.end(),
                              glyph_outlines[exterior_outline_id].curves.begin(),
                              glyph_outlines[exterior_outline_id].curves.end());

        std::function<void(size_t, int)> collect_holes = [&](const size_t parent_idx,
                                                             const size_t parent_level) {
            for (size_t child_id : nodes[parent_idx].children) {
                const size_t child_level = nodes[child_id].nesting_level;
                if (child_level == parent_level + 1) {

                    const float area =
                        math::signed_area(glyph_outlines[child_id].vertices);

                    if (abs(area) < std::numeric_limits<float>::epsilon()) {
                        continue;
                    } else if (font::is_counter_clockwise_winding(area)) {
                        std::ranges::reverse(glyph_outlines[child_id].vertices);
                    }

                    polygon.interior_outlines.push_back(
                        std::move(glyph_outlines[child_id].vertices));

                    polygon.curves.insert(polygon.curves.end(),
                                          glyph_outlines[child_id].curves.begin(),
                                          glyph_outlines[child_id].curves.end());
                }
                collect_holes(child_id, child_level);
            }
        };

        collect_holes(exterior_outline_id, 0);

        polygons.push_back(std::move(polygon));
    }

    return polygons;
}
