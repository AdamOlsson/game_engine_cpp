#pragma once

#include "../EntitySettingsPanel.h"
#include "../GameState.h"
#include "camera/Camera.h"
#include "state_machine/StateTransition.h"
#include <vector>

class DefendState {
  private:
    EntitySettingsPanel m_settings_panel;

    constexpr void spawn_group_of_enemies(GameState &game_state);

    std::optional<size_t> find_caravan_cart(GameState &game_state,
                                            const camera::WorldPoint2D &point);
    std::optional<size_t>
    find_selected_caravan_slot(GameState &game_state,
                               const camera::WorldPoint2D &click_point);
    std::optional<size_t> find_selected_guard(GameState &game_state,
                                              const camera::WorldPoint2D &click_point);

    template <typename T> void update_all(const float dt, std::vector<T> &vec) {
        for (T &t : vec) {
            t.update(dt);
        }
    }

    void handle_click(GameState &game_state, interface::ViewportPoint &click_position);
    void handle_hover(GameState &game_state, interface::ViewportPoint &cursor_position);
    void handle_cursor(const interface::NDCPoint &cursor_position,
                       const bool has_clicked);

  public:
    DefendState() = default;

    DefendState(const font::Font &font) : m_settings_panel(EntitySettingsPanel(font)) {}

    DefendState(DefendState &&) noexcept = default;
    DefendState(const DefendState &) = delete;
    DefendState &operator=(DefendState &&) = default;
    DefendState &operator=(const DefendState &) = delete;

    ~DefendState() {}

    void on_enter(GameState &game_state);
    void on_exit(GameState &game_state);
    util::StateTransition update(const float dt, GameState &game_state);

    std::vector<font::TextFormat> get_ui_text_format_render_data() {
        std::vector<font::TextFormat> text_format;
        std::vector<font::Text> text;
        std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> geometry_data;
        if (m_settings_panel.is_open()) {
            m_settings_panel.get_render_data(text_format, text, geometry_data);
        }
        return text_format;
    }

    std::vector<font::Text> get_ui_text_render_data() {
        std::vector<font::TextFormat> text_format;
        std::vector<font::Text> text;
        std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> geometry_data;
        if (m_settings_panel.is_open()) {
            m_settings_panel.get_render_data(text_format, text, geometry_data);
        }
        return text;
    }

    std::vector<graphics_pipeline::geometry::GeometryPipelineSBO>
    get_ui_geometry_render_data() {
        std::vector<font::TextFormat> text_format;
        std::vector<font::Text> text;
        std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> geometry_data;
        if (m_settings_panel.is_open()) {
            m_settings_panel.get_render_data(text_format, text, geometry_data);
        }
        return geometry_data;
    }

    std::vector<graphics_pipeline::quad::QuadPipelineSBO>
    get_quad_render_data(const GameState &game_state) {
        std::vector<graphics_pipeline::quad::QuadPipelineSBO> quad_data;
        quad_data.reserve(game_state.guards.size() + game_state.caravan.size() +
                          game_state.enemies.size());

        for (auto &entity : game_state.caravan) {
            entity.get_quad_render_data(quad_data);
        }

        for (auto &entity : game_state.caravan_slots) {
            entity.get_quad_render_data(quad_data);
        }

        for (auto &entity : game_state.guards) {
            entity.get_quad_render_data(quad_data);
        }

        for (auto &entity : game_state.enemies) {
            entity.get_quad_render_data(quad_data);
        }

        for (auto &entity : game_state.attacks) {
            if (entity.is_visible()) {
                entity.get_quad_render_data(quad_data);
            }
        }

        return quad_data;
    }

    std::vector<graphics_pipeline::geometry::GeometryPipelineSBO>
    get_geometry_render_data(const GameState &game_state) {
        std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> geometry_data;
        geometry_data.reserve(game_state.enemies.size());

        for (auto &entity : game_state.guards) {
            entity.get_geometry_render_data(geometry_data);
        }

        for (auto &entity : game_state.caravan) {
            entity.get_geometry_render_data(geometry_data);
        }

        for (auto &entity : game_state.enemies) {
            entity.get_geometry_render_data(geometry_data);
        }

        return geometry_data;
    }
};
