#pragma once

#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "graphics_pipeline/text/TextRenderer.h"
#include "interface/NDCPosition.h"
#include <vector>

using DialogOptionCb = std::function<void()>;

struct DialogOption {
    std::string label;
    graphics_pipeline::text::TextHandle text_handle;
    graphics_pipeline::geometry::GeometryPipelineSBO bbox_render_data;

    std::optional<std::string> next_dialog_node;
    std::function<void()> on_click = []() {};

    bool is_inside(const interface::NDCPoint &point) {
        return text_handle.is_point_inside(point);
    }
};

struct DialogNode {
    std::string id;
    graphics_pipeline::text::TextHandle text_handle;

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
