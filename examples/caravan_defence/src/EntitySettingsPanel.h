#pragma once

#include "DropDown.h"
#include "font/types.h"
#include "graphics_pipeline/geometry/GeometryPipelineSBO.h"
#include "util/colors.h"

class EntitySettingsPanel {
  private:
    static constexpr util::colors::Color background_color =
        util::colors::rgba(0.02f, 0.02f, 0.02f, 0.8f);
    static constexpr util::colors::Color hover_color =
        util::colors::rgba(0.08f, 0.08f, 0.08f, 0.90f);
    static constexpr util::colors::Color border_color = util::colors::hex(0x8bac0f);
    static constexpr util::colors::Color font_color = util::colors::hex(0x9bbc0f);

    graphics_pipeline::geometry::GeometryPipelineSBO m_bbox_render_data;

    font::TextFormat m_headline_format;
    font::Text m_headline;

    float m_center_x = 0.0f;
    math::Vector2 m_size;

    std::vector<DropDown> m_drop_downs;

    bool m_is_open = false;

  public:
    EntitySettingsPanel() = default;
    EntitySettingsPanel(const font::Font &font) {

        const float bbox_center_line_x = -0.7f;

        m_center_x = bbox_center_line_x;
        m_is_open = false;
        m_size = math::Vector2(0.6f, 2.0f);

        m_bbox_render_data.color = EntitySettingsPanel::background_color;
        m_bbox_render_data.border.color = EntitySettingsPanel::border_color;
        m_bbox_render_data.border.width = 0.015f;
        m_bbox_render_data.border.radius = 0.05f;
        m_bbox_render_data.model_matrix =
            math::Matrix().translate(m_center_x, 0.0f).scale(m_size);

        auto text_opts = font::TextOpts{};
        text_opts.font_color = EntitySettingsPanel::font_color;
        text_opts.font_size = 0.05f;
        text_opts.line_width = 0.20f;
        text_opts.position =
            math::Vector2(m_center_x - text_opts.line_width / 2.0f, -0.90f);

        m_headline_format = font.create_text_format(text_opts);
        m_headline = font.create_text("Guard", text_opts);

        DropDown select_weapon_drop_down{};
        select_weapon_drop_down.bbox.color = DropDown::background_color;
        select_weapon_drop_down.bbox.border.color = EntitySettingsPanel::border_color;
        select_weapon_drop_down.bbox.border.width = 0.005;
        select_weapon_drop_down.bbox.model_matrix =
            math::Matrix().translate(m_center_x, -0.8f).scale(0.4f, 0.05f);

        const auto change_weapon_to_sniper = [](GameState &game_state) {
            DEBUG_ASSERT(
                game_state.selected_guard.has_value(),
                "Error: Settings panel is open for a guard, but no guard is selected.");
            game_state.guards[game_state.selected_guard.value()].set_weapon(
                Weapon::create_weapon<Sniper>());
        };

        const auto change_weapon_to_bow = [](GameState &game_state) {
            DEBUG_ASSERT(
                game_state.selected_guard.has_value(),
                "Error: Settings panel is open for a guard, but no guard is selected.");
            game_state.guards[game_state.selected_guard.value()].set_weapon(
                Weapon::create_weapon<Bow>());
        };

        const auto change_weapon_to_sword = [](GameState &game_state) {
            DEBUG_ASSERT(
                game_state.selected_guard.has_value(),
                "Error: Settings panel is open for a guard, but no guard is selected.");
            game_state.guards[game_state.selected_guard.value()].set_weapon(
                Weapon::create_weapon<Sword>());
        };

        select_weapon_drop_down.add_headline(font, "Select weapon");
        select_weapon_drop_down.add_drop_down_item(font, "Sniper",
                                                   change_weapon_to_sniper);
        select_weapon_drop_down.add_drop_down_item(font, "Bow", change_weapon_to_bow);
        select_weapon_drop_down.add_drop_down_item(font, "Sword", change_weapon_to_sword);
        m_drop_downs.push_back(std::move(select_weapon_drop_down));
    }

    void get_render_data(
        std::vector<font::TextFormat> &text_format_out2,
        std::vector<font::Text> &text_out2,
        std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> &geom_out) {

        geom_out.push_back(m_bbox_render_data);

        text_format_out2.push_back(m_headline_format);
        text_out2.push_back(m_headline);

        if (m_drop_downs.size() > 0) {
            for (auto &dd : m_drop_downs) {
                text_format_out2.push_back(dd.headline_format);
                text_out2.push_back(dd.headline);
                if (dd.is_open) {
                    for (auto &item : dd.items) {
                        geom_out.push_back(item.bbox);
                        text_format_out2.push_back(item.text_format);
                        text_out2.push_back(item.text);
                    }
                } else {
                    geom_out.push_back(dd.bbox);
                }
            }
            return;
        }
    }

    bool is_point_inside(const math::Vector2 &point) const {
        const camera::WorldPoint2D position =
            math::Matrix::position_2d(m_bbox_render_data.model_matrix);
        return math::is_point_inside_rectangle(point, position, m_size.x(), m_size.y());
    }

    void handle_cursor(GameState &game_state) {
        if (!m_is_open) {
            return;
        }

        const interface::NDCPoint cursor_position =
            game_state.camera.to_ndc_point(game_state.cursor.viewport_position);
        const bool has_clicked = game_state.cursor.click_point.has_value();

        if (has_clicked && is_point_inside(cursor_position)) {
            bool click_inside_any_dropdown = false;
            for (auto &dd : m_drop_downs) {
                if (dd.is_point_inside(cursor_position)) {
                    click_inside_any_dropdown = true;
                    break;
                }
            }
            if (!click_inside_any_dropdown) {
                close_drop_downs();
                return;
            }
        }

        for (auto &dd : m_drop_downs) {
            dd.handle_cursor(game_state);
        }
    }

    bool is_open() const { return m_is_open; }
    void open() { m_is_open = true; }
    void close() {
        m_is_open = false;
        for (auto &dd : m_drop_downs) {
            dd.close();
        }
    }
    void close_drop_downs() {
        for (auto &dd : m_drop_downs) {
            dd.close();
        }
    }
};
