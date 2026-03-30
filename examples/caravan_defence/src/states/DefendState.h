#pragma once

#include "../DropDown.h"
#include "../GameState.h"
#include "camera/Camera.h"
#include "graphics_pipeline/geometry/GeometryRenderer2.h"
#include "graphics_pipeline/quad/QuadRenderer2.h"
#include "graphics_pipeline/text/TextRenderer.h"
#include "graphics_pipeline/text/TextRenderer2.h"
#include "state_machine/StateTransition.h"
#include <vector>

struct EntitySettingsPanel {
    static constexpr util::colors::Color background_color =
        util::colors::rgba(0.02f, 0.02f, 0.02f, 0.8f);
    static constexpr util::colors::Color hover_color =
        util::colors::rgba(0.08f, 0.08f, 0.08f, 0.90f);
    static constexpr util::colors::Color border_color = util::colors::hex(0x8bac0f);
    static constexpr util::colors::Color font_color = util::colors::hex(0x9bbc0f);

    graphics_pipeline::geometry::GeometryPipelineSBO bbox_render_data;

    font::TextFormat headline_format;
    font::Text headline;

    float center_x = 0.0f;
    math::Vector2 size;

    std::vector<DropDown> drop_downs;

    bool is_open = false;

    void get_render_data(
        std::vector<graphics_pipeline::text::TextHandle *> &text_out,
        std::vector<font::TextFormat> &text_format_out2,
        std::vector<font::Text> &text_out2,
        std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> &geom_out) {

        geom_out.push_back(bbox_render_data);

        text_format_out2.push_back(headline_format);
        text_out2.push_back(headline);

        if (drop_downs.size() > 0) {
            for (auto &dd : drop_downs) {
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
            math::Matrix::position_2d(bbox_render_data.model_matrix);
        return math::is_point_inside_rectangle(point, position, size.x(), size.y());
    }

    void handle_cursor(GameState &game_state) {
        if (!is_open) {
            return;
        }

        const interface::NDCPoint cursor_position =
            game_state.camera.to_ndc_point(game_state.cursor.viewport_position);
        const bool has_clicked = game_state.cursor.click_point.has_value();

        if (has_clicked && is_point_inside(cursor_position)) {
            bool click_inside_any_dropdown = false;
            for (auto &dd : drop_downs) {
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

        for (auto &dd : drop_downs) {
            dd.handle_cursor(game_state);
        }
    }

    void open() { is_open = true; }
    void close() {
        is_open = false;
        for (auto &dd : drop_downs) {
            dd.close();
        }
    }
    void close_drop_downs() {
        for (auto &dd : drop_downs) {
            dd.close();
        }
    }
};

class DefendState {
  private:
    // World renderers
    graphics_pipeline::quad::QuadRenderer2 *m_world_quad_renderer = nullptr;
    graphics_pipeline::geometry::GeometryRenderer2 *m_world_geometry_renderer = nullptr;

    // UI renderers
    graphics_pipeline::text::TextRenderer2 *m_ui_text_renderer2 = nullptr;
    graphics_pipeline::geometry::GeometryRenderer2 *m_ui_geometry_renderer = nullptr;

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

    EntitySettingsPanel init_entity_settings_panel();

  public:
    DefendState() = default;

    DefendState(graphics_pipeline::quad::QuadRenderer2 *world_quad_renderer,
                graphics_pipeline::geometry::GeometryRenderer2 *world_geom_renderer,
                graphics_pipeline::text::TextRenderer2 *ui_text_renderer2,
                graphics_pipeline::geometry::GeometryRenderer2 *ui_geom_renderer)
        : m_world_quad_renderer(world_quad_renderer),
          m_world_geometry_renderer(world_geom_renderer),
          m_ui_text_renderer2(ui_text_renderer2),
          m_ui_geometry_renderer(ui_geom_renderer) {
        m_settings_panel = init_entity_settings_panel();
    }

    DefendState(DefendState &&) noexcept = default;
    DefendState(const DefendState &) = delete;
    DefendState &operator=(DefendState &&) = default;
    DefendState &operator=(const DefendState &) = delete;

    ~DefendState() {}

    void on_enter(GameState &game_state);
    void on_exit(GameState &game_state);
    util::StateTransition update(const float dt, GameState &game_state);

    template <typename PushConstantType>
    void render_ui(const vulkan::CommandBuffer &command_buffer,
                   PushConstantType *push_constant) {

        if (m_settings_panel.is_open) {

            std::vector<graphics_pipeline::text::TextHandle *> text_handles;
            std::vector<font::TextFormat> text_format;
            std::vector<font::Text> text;
            std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> geometry_data;

            m_settings_panel.get_render_data(text_handles, text_format, text,
                                             geometry_data);

            vulkan::DrawIndexedIndirectCommand geom_draw_command =
                m_ui_geometry_renderer->write_to_buffer(geometry_data);
            m_ui_geometry_renderer->render_indirect(command_buffer, push_constant,
                                                    {geom_draw_command});

            size_t offset = 0;
            std::vector<vulkan::DrawIndexedIndirectCommand> text_draw_commands;
            for (size_t i = 0; i < text.size(); i++) {
                size_t format_idx = i;
                m_ui_text_renderer2->write_to_format_buffer({text_format[i]}, format_idx);
                auto draw_cmd = m_ui_text_renderer2->write_to_glyph_buffer(
                    text[i], format_idx, offset);
                offset += text[i].glyphs.size();

                text_draw_commands.insert(text_draw_commands.end(), draw_cmd.begin(),
                                          draw_cmd.end());
            }

            m_ui_text_renderer2->render_indirect(command_buffer, push_constant,
                                                 text_draw_commands);
        }
    }

    template <typename PushConstantType>
    void render(const vulkan::CommandBuffer &command_buffer,
                PushConstantType *push_constant, const GameState &game_state) {

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

        vulkan::DrawIndexedIndirectCommand quad_draw_command =
            m_world_quad_renderer->write_to_buffer(quad_data);

        std::vector<vulkan::DrawIndexedIndirectCommand> quad_draw_commands;
        quad_draw_commands.push_back(quad_draw_command);

        m_world_quad_renderer->render_indirect(command_buffer, push_constant,
                                               quad_draw_commands);

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

        vulkan::DrawIndexedIndirectCommand geometry_draw_command =
            m_world_geometry_renderer->write_to_buffer(geometry_data);

        std::vector<vulkan::DrawIndexedIndirectCommand> geometry_draw_commands;
        geometry_draw_commands.push_back(geometry_draw_command);

        m_world_geometry_renderer->render_indirect(command_buffer, push_constant,
                                                   geometry_draw_commands);
    }
};
