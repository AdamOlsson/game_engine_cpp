#pragma once

#include "camera/Camera.h"
#include "graphics_pipeline/geometry/GeometryRenderer.h"
#include "graphics_pipeline/quad/QuadRenderer.h"
#include "graphics_pipeline/text/TextRenderer.h"
#include "state_machine/StateTransition.h"
#include <vector>

struct GameState;

struct EntitySettingsPanel {
    static constexpr util::colors::Color background_color =
        util::colors::rgba(0.02f, 0.02f, 0.02f, 0.8f);
    static constexpr util::colors::Color border_color = util::colors::hex(0x8bac0f);
    static constexpr util::colors::Color font_color = util::colors::hex(0x9bbc0f);

    graphics_pipeline::geometry::GeometrySBOHandle bbox_handle;
    std::vector<graphics_pipeline::text::TextHandle> text_handles;

    bool is_open = false;
};

class DefendState {
  private:
    // World renderers
    graphics_pipeline::quad::QuadRenderer *m_world_quad_renderer = nullptr;
    graphics_pipeline::geometry::GeometryRenderer *m_world_geometry_renderer = nullptr;

    // UI renderers
    graphics_pipeline::text::TextRenderer *m_ui_text_renderer = nullptr;
    graphics_pipeline::geometry::GeometryRenderer *m_ui_geometry_renderer = nullptr;

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

    void handle_cursor(GameState &game_state, interface::ViewportPoint &click_point);

    EntitySettingsPanel init_entity_settings_panel();
    void open_entity_settings_panel();
    void close_entity_settings_panel();

  public:
    DefendState() = default;

    DefendState(graphics_pipeline::quad::QuadRenderer *world_quad_renderer,
                graphics_pipeline::geometry::GeometryRenderer *world_geom_renderer,
                graphics_pipeline::text::TextRenderer *ui_text_renderer,
                graphics_pipeline::geometry::GeometryRenderer *ui_geom_renderer)
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
    void render_text(const vulkan::CommandBuffer &command_buffer,
                     PushConstantType *push_constant) {
        if (m_settings_panel.is_open) {
            for (auto &text : m_settings_panel.text_handles) {
                m_ui_text_renderer->render(command_buffer, text, push_constant);
            }
        }
    }
};
