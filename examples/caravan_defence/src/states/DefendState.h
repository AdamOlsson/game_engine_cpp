#pragma once

#include "../GameState.h"
#include "camera/Camera.h"
#include "graphics_pipeline/geometry/GeometryRenderer2.h"
#include "graphics_pipeline/quad/QuadRenderer2.h"
#include "graphics_pipeline/text/TextRenderer.h"
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
    std::vector<graphics_pipeline::text::TextHandle> text_handles;

    float center_x = 0.0f;
    math::Vector2 size;

    struct DropDown {

        struct DropDownItem {
            static constexpr util::colors::Color hover_color =
                util::colors::rgba(1.0f, 0.05f, 0.05f, 0.90f);

            graphics_pipeline::geometry::GeometryPipelineSBO bbox;

            bool is_point_inside(const math::Vector2 &point) const {
                const math::Vector2 position =
                    math::Matrix::position_2d(bbox.model_matrix);
                const math::Vector2 size = math::Matrix::scale_2d_axis(bbox.model_matrix);
                return math::is_point_inside_rectangle(point, position, size.x(),
                                                       size.y());
            }

            void apply_hover_effects() { bbox.color = EntitySettingsPanel::hover_color; }
            void remove_hover_effects() {
                bbox.color = EntitySettingsPanel::background_color;
            }
        };

        static constexpr util::colors::Color background_color =
            util::colors::rgba(0.03f, 0.03f, 0.03f, 0.90f);

        graphics_pipeline::geometry::GeometryPipelineSBO bbox;

        bool is_open = false;

        std::vector<DropDownItem> items;

        size_t selected_id = std::numeric_limits<size_t>::max();
        size_t hovered_item_id = std::numeric_limits<size_t>::max();

        void add_drop_down_item() {
            const math::Vector2 parent_position =
                math::Matrix::position_2d(bbox.model_matrix);
            const math::Vector2 parent_size =
                math::Matrix::scale_2d_axis(bbox.model_matrix);

            const float drop_down_item_y_pos =
                parent_position.y() + parent_size.y() * items.size();

            DropDownItem item{};
            item.bbox.color = EntitySettingsPanel::DropDown::background_color;
            item.bbox.border.color =
                items.size() % 2 == 0 ? util::colors::BLUE : util::colors::RED;
            item.bbox.border.width = 0.005f;
            item.bbox.model_matrix =
                math::Matrix()
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

        void on_click(const interface::NDCPoint &point) {
            for (auto &item : items) {
                if (item.is_point_inside(point)) {
                    close();
                }
            }
        }

        void handle_cursor(const interface::NDCPoint &cursor_position,
                           const bool has_clicked) {
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
                    on_click(cursor_position);
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

        void apply_hover_effects() { bbox.color = EntitySettingsPanel::hover_color; }
        void remove_hover_effects() { bbox.color = background_color; }
    };

    std::vector<graphics_pipeline::text::TextHandle> drop_down_headers;
    std::vector<DropDown> drop_downs;

    bool is_open = false;

    std::vector<graphics_pipeline::geometry::GeometryPipelineSBO>
    get_geometry_render_data() {
        std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> item_render_data{
            bbox_render_data};

        if (!is_open) {
            return item_render_data;
        }

        if (drop_downs.size() > 0) {
            for (auto &dd : drop_downs) {
                if (dd.is_open) {
                    for (const auto &item : dd.items) {
                        item_render_data.push_back(item.bbox);
                    }
                } else {
                    item_render_data.push_back(dd.bbox);
                }
            }
            return item_render_data;
        }
        return item_render_data;
    }

    bool is_point_inside(const math::Vector2 &point) const {
        const camera::WorldPoint2D position =
            math::Matrix::position_2d(bbox_render_data.model_matrix);
        return math::is_point_inside_rectangle(point, position, size.x(), size.y());
    }

    void handle_cursor(const interface::NDCPoint &cursor_position,
                       const bool has_clicked) {
        if (!is_open) {
            return;
        }

        for (auto &dd : drop_downs) {
            dd.handle_cursor(cursor_position, has_clicked);
        }
    }

    void open() { is_open = true; }
    void close() { is_open = false; }
};

class DefendState {
  private:
    // World renderers
    graphics_pipeline::quad::QuadRenderer2 *m_world_quad_renderer = nullptr;
    graphics_pipeline::geometry::GeometryRenderer2 *m_world_geometry_renderer = nullptr;

    // UI renderers
    graphics_pipeline::text::TextRenderer *m_ui_text_renderer = nullptr;
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
                graphics_pipeline::text::TextRenderer *ui_text_renderer,
                graphics_pipeline::geometry::GeometryRenderer2 *ui_geom_renderer)
        : m_world_quad_renderer(world_quad_renderer),
          m_world_geometry_renderer(world_geom_renderer),
          m_ui_text_renderer(ui_text_renderer), m_ui_geometry_renderer(ui_geom_renderer) {
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

            std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> geometry_data =
                m_settings_panel.get_geometry_render_data();

            vulkan::DrawIndexedIndirectCommand draw_command =
                m_ui_geometry_renderer->write_to_buffer(geometry_data);
            m_ui_geometry_renderer->render_indirect(command_buffer, push_constant,
                                                    {draw_command});

            std::vector<vulkan::DrawIndexedIndirectCommand> draw_commands;
            draw_commands.push_back(draw_command);
            for (auto &text : m_settings_panel.text_handles) {
                m_ui_text_renderer->render(command_buffer, text, push_constant);
            }
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
