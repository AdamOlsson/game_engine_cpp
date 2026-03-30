#pragma once

#include "GameState.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "graphics_pipeline/text/TextRenderer2.h"
#include "util/colors.h"
struct DropDown {

    struct DropDownItem {
        graphics_pipeline::geometry::GeometryPipelineSBO bbox;
        font::TextFormat text_format;
        font::Text text;

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

    font::TextFormat headline_format;
    font::Text headline;

    graphics_pipeline::geometry::GeometryPipelineSBO bbox;

    bool is_open = false;

    std::vector<DropDownItem> items;

    size_t selected_item_id = std::numeric_limits<size_t>::max();
    size_t hovered_item_id = std::numeric_limits<size_t>::max();

    void add_headline(graphics_pipeline::text::TextRenderer2 *text_renderer2,
                      const std::string &text) {

        const math::Vector2 drop_down_position =
            math::Matrix::position_2d(bbox.model_matrix);
        const math::Vector2 drop_down_size =
            math::Matrix::scale_2d_axis(bbox.model_matrix);

        const float headline_pos_y = drop_down_position.y() - drop_down_size.y();

        auto text_opts = font::TextOpts{};
        text_opts.font_color = font_color;
        text_opts.font_size = 0.03f;
        text_opts.line_width = drop_down_size.x();
        text_opts.line_height = drop_down_size.y();

        text_opts.position = math::Vector2(
            drop_down_position.x() - drop_down_size.x() / 2.0f, headline_pos_y);
        headline_format = text_renderer2->m_font.create_text_format(text_opts);
        headline = text_renderer2->m_font.create_text(text, text_opts);
    }

    void add_drop_down_item(graphics_pipeline::text::TextRenderer2 *text_renderer2,
                            const std::string &text,
                            std::function<void(GameState &)> on_click_cb) {
        const math::Vector2 parent_position =
            math::Matrix::position_2d(bbox.model_matrix);
        const math::Vector2 parent_size = math::Matrix::scale_2d_axis(bbox.model_matrix);

        const float drop_down_item_y_pos =
            parent_position.y() + parent_size.y() * items.size();

        auto text_opts = font::TextOpts{};
        text_opts.font_color = font_color;
        text_opts.font_size = 0.03f;
        text_opts.line_width = parent_size.x();
        text_opts.line_height = parent_size.y();
        text_opts.position = math::Vector2(parent_position.x() - parent_size.x() / 2.0f,
                                           drop_down_item_y_pos);

        DropDownItem item{};
        item.text_format = text_renderer2->m_font.create_text_format(text_opts);
        item.text = text_renderer2->m_font.create_text(text, text_opts);
        item.on_click_cb = on_click_cb;
        item.bbox.color = background_color;
        item.bbox.border.width = 0.005;
        item.bbox.model_matrix = math::Matrix()
                                     .translate(parent_position.x(), drop_down_item_y_pos)
                                     .scale(parent_size);

        items.push_back(std::move(item));
    }

    bool is_point_inside(const math::Vector2 &point) const {
        const math::Vector2 position = math::Matrix::position_2d(bbox.model_matrix);
        const math::Vector2 size = math::Matrix::scale_2d_axis(bbox.model_matrix);

        if (!is_open) {
            return math::is_point_inside_rectangle(point, position, size.x(), size.y());
        }

        const size_t num_items = items.size();
        const math::Vector2 open_size = math::Vector2(size.x(), size.y() * num_items);

        float open_mid_point_y = 0.0f;
        for (const auto &item : items) {
            open_mid_point_y += math::Matrix::position_2d(item.bbox.model_matrix).y();
        }
        open_mid_point_y /= num_items;
        const math::Vector2 open_mid_point =
            math::Vector2(position.x(), open_mid_point_y);
        return math::is_point_inside_rectangle(point, open_mid_point, open_size.x(),
                                               open_size.y());
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
            if (has_clicked && hovered_item_id < items.size()) {
                selected_item_id = hovered_item_id;
                items[selected_item_id].on_click_cb(game_state);
                close();
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
