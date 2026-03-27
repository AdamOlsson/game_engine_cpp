#pragma once

#include "GameState.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "util/colors.h"
struct DropDown {

    struct DropDownItem {
        static constexpr util::colors::Color hover_color =
            util::colors::rgba(1.0f, 0.05f, 0.05f, 0.90f);

        graphics_pipeline::geometry::GeometryPipelineSBO bbox;

        std::function<void(GameState &)> on_click_cb = [](GameState &) {};

        bool is_point_inside(const math::Vector2 &point) const {
            const math::Vector2 position = math::Matrix::position_2d(bbox.model_matrix);
            const math::Vector2 size = math::Matrix::scale_2d_axis(bbox.model_matrix);
            return math::is_point_inside_rectangle(point, position, size.x(), size.y());
        }

        void apply_hover_effects() { bbox.color = hover_color; }
        void remove_hover_effects() { bbox.color = background_color; }
    };

    static constexpr util::colors::Color background_color =
        util::colors::rgba(0.03f, 0.03f, 0.03f, 0.90f);
    static constexpr util::colors::Color hover_color =
        util::colors::rgba(0.08f, 0.08f, 0.08f, 0.90f);
    static constexpr util::colors::Color border_color = util::colors::hex(0x8bac0f);
    static constexpr util::colors::Color font_color = util::colors::hex(0x9bbc0f);

    graphics_pipeline::geometry::GeometryPipelineSBO bbox;

    bool is_open = false;

    std::vector<DropDownItem> items;

    size_t selected_id = std::numeric_limits<size_t>::max();
    size_t hovered_item_id = std::numeric_limits<size_t>::max();

    void add_drop_down_item(std::function<void(GameState &)> on_click_cb) {
        const math::Vector2 parent_position =
            math::Matrix::position_2d(bbox.model_matrix);
        const math::Vector2 parent_size = math::Matrix::scale_2d_axis(bbox.model_matrix);

        const float drop_down_item_y_pos =
            parent_position.y() + parent_size.y() * items.size();

        DropDownItem item{};
        item.on_click_cb = on_click_cb;
        item.bbox.color = background_color;
        item.bbox.border.color =
            items.size() % 2 == 0 ? util::colors::BLUE : util::colors::RED;
        item.bbox.border.width = 0.005f;
        item.bbox.model_matrix = math::Matrix()
                                     .translate(parent_position.x(), drop_down_item_y_pos)
                                     .scale(parent_size);

        items.push_back(item);
    }

    bool is_point_inside(const math::Vector2 &point) const {
        const camera::WorldPoint2D position =
            math::Matrix::position_2d(bbox.model_matrix);
        const math::Vector2 size = math::Matrix::scale_2d_axis(bbox.model_matrix);
        return math::is_point_inside_rectangle(point, position, size.x(), size.y());
    }

    void handle_cursor(GameState &game_state) {

        const interface::NDCPoint cursor_position =
            game_state.camera.to_ndc_point(game_state.cursor.viewport_position);
        const bool has_clicked = game_state.cursor.click_point.has_value();

        if (is_open) {
            bool item_hovered = false;
            for (size_t i = 0; i < items.size(); ++i) {
                if (items[i].is_point_inside(cursor_position)) {
                    if (hovered_item_id != i) {
                        if (hovered_item_id < items.size()) {
                            items[hovered_item_id].remove_hover_effects();
                        }
                        items[i].apply_hover_effects();
                        hovered_item_id = i;
                    }
                    item_hovered = true;
                    break;
                }
            }
            if (!item_hovered && hovered_item_id < items.size()) {
                items[hovered_item_id].remove_hover_effects();
                hovered_item_id = std::numeric_limits<size_t>::max();
            }
            if (has_clicked) {
                items[hovered_item_id].on_click_cb(game_state);
            }
        } else {
            if (is_point_inside(cursor_position)) {
                apply_hover_effects();
                if (has_clicked) {
                    open();
                }
            } else {
                remove_hover_effects();
            }
        }
    }

    void open() { is_open = true; }
    void close() {
        is_open = false;
        hovered_item_id = std::numeric_limits<size_t>::max();
    }

    void apply_hover_effects() { bbox.color = hover_color; }
    void remove_hover_effects() { bbox.color = background_color; }
};
