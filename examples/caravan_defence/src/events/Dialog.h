#pragma once

#include "font/types.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "interface/NDCPosition.h"
#include "math/shape.h"
#include <vector>

using DialogOptionCb = std::function<void()>;

struct DialogOption {
    std::string label;

    font::TextFormat text_format;
    font::Text text;

    graphics_pipeline::geometry::GeometryPipelineSBO bbox_render_data;

    std::optional<std::string> next_dialog_node;
    std::function<void()> on_click = []() {};

    bool is_inside(const interface::NDCPoint &point) {
        const auto &bbox = text.bbox;
        const math::Vector2 center = bbox.center();
        const math::Vector2 size = bbox.size();
        return math::is_point_inside_rectangle(point, center, size.x(), size.y());
    }
};

struct DialogNode {
    std::string id;

    font::TextFormat text_format;
    font::Text text;

    std::vector<DialogOption> options;

    size_t get_option(const interface::NDCPoint &point) {
        size_t id = std::numeric_limits<size_t>::max();
        for (size_t i = 0; i < options.size(); i++) {
            auto &option = options[i];
            if (option.is_inside(point)) {
                id = i;
                break;
            }
        }
        return id;
    }
};
