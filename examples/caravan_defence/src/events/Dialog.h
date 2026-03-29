#pragma once

#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "graphics_pipeline/text/TextRenderer.h"
#include "graphics_pipeline/text/TextRenderer2.h"
#include "interface/NDCPosition.h"
#include <vector>

using DialogOptionCb = std::function<void()>;

struct DialogOption {
    std::string label;

    graphics_pipeline::text::TextFormatSBO2 text_format;
    std::vector<graphics_pipeline::text::TextGlyphSBO2> text_glyphs;

    graphics_pipeline::text::TextHandle text_handle; // TODO: Remove
    graphics_pipeline::geometry::GeometryPipelineSBO bbox_render_data;

    std::optional<std::string> next_dialog_node;
    std::function<void()> on_click = []() {};

    // TODO: refactor
    bool is_inside(const interface::NDCPoint &point) {
        return text_handle.is_point_inside(point);
    }
};

struct DialogNode {
    std::string id;

    /*graphics_pipeline::text::TextFormatSBO2 text_format;*/
    font::TextFormat text_format;
    font::Text text;

    /*graphics_pipeline::text::TextHandle text_handle; // TODO: Remove*/

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
