#pragma once

#include "Configuration.h"
#include "Entity.h"
#include "EventFactory.h"
#include "GameState.h"
#include "camera/Camera.h"
#include "events/Event.h"
#include "game_engine_sdk/Game.h"
#include "graphics_pipeline/quad/QuadRenderer.h"
#include "graphics_pipeline/text/TextRenderer.h"

#include "states/state_machine/StateMachine.h"
#include "states/types.h"
#include "vulkan/CommandBufferManager.h"

#define ASSET_FILE(filename) ASSET_DIR "/" filename
constexpr glm::vec2 INVERT_AXISES = glm::vec2(-1.0f, -1.0f);
constexpr float ZOOM_SCALE_FACTOR = 0.1f;

constexpr vulkan::RenderPassOpts world_pass =
    vulkan::RenderPassOpts{.color = {
                               .load_op = VK_ATTACHMENT_LOAD_OP_CLEAR,
                               .store_op = VK_ATTACHMENT_STORE_OP_STORE,
                               .initial_layout = VK_IMAGE_LAYOUT_UNDEFINED,
                               .final_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                           }};

constexpr vulkan::RenderPassOpts ui_pass =
    vulkan::RenderPassOpts{.color = {
                               .load_op = VK_ATTACHMENT_LOAD_OP_LOAD,
                               .store_op = VK_ATTACHMENT_STORE_OP_STORE,
                               .initial_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                               .final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                           }};

// CONTINUE: Make states more independent
// - move out event member from game state and into EventState
class CaravanDefence : public Game {
  private:
    friend class Configuration;

    std::unique_ptr<vulkan::SwapChain> m_swap_chain;

    std::unique_ptr<vulkan::CommandBufferManager> m_command_buffer_manager;

    // World renderers
    vulkan::RenderPass m_world_render_pass;
    std::unique_ptr<graphics_pipeline::quad::QuadRenderer> m_quad_renderer;
    std::unique_ptr<graphics_pipeline::geometry::GeometryRenderer> m_geom_renderer;

    // UI renderers
    vulkan::RenderPass m_ui_render_pass;
    std::unique_ptr<graphics_pipeline::text::TextRenderer> m_ui_text_renderer;
    std::unique_ptr<graphics_pipeline::geometry::GeometryRenderer> m_ui_geom_renderer;

    struct GameState m_game_state;
    util::StateMachine<CaravanDefenceStates, GameState> m_state_machine;

  public:
    CaravanDefence() {}

    ~CaravanDefence() {};

    void setup(std::shared_ptr<vulkan::context::GraphicsContext> &ctx) override {

        auto window_size = ctx->window->get_framebuffer_size<float>();
        m_swap_chain = std::make_unique<vulkan::SwapChain>(ctx);

        m_world_render_pass = m_swap_chain->create_render_pass(world_pass);
        m_ui_render_pass = m_swap_chain->create_render_pass(ui_pass);

        m_command_buffer_manager = std::make_unique<vulkan::CommandBufferManager>(ctx, 2);

        m_game_state.camera = camera::Camera2D(window_size.width, window_size.height);
        m_game_state.camera.set_zoom(0.1f);

        // renderers
        Configuration::setup_world_renderers(ctx, *this);
        Configuration::setup_ui_renderers(ctx, *this);

        Configuration::setup_mouse_event_handler(ctx, *this);
        Configuration::setup_keyboard_event_handler(ctx, *this);

        Configuration::setup_initial_game_state(m_geom_renderer.get(),
                                                m_quad_renderer.get(), m_game_state);

        m_state_machine.get_state<DefendState>() =
            DefendState(m_quad_renderer.get(), m_geom_renderer.get());
        m_state_machine.get_state<EventState>() =
            EventState(m_ui_text_renderer.get(), m_ui_geom_renderer.get());

        m_state_machine.init<DefendState>();
    }

    void update(const float dt) override { m_state_machine.update(dt, m_game_state); };

    void render() override {

        const camera::WorldPoint2D cursor_world_point =
            m_game_state.camera.viewport_to_world(m_game_state.cursor.viewport_position);

        // Only highlight slots when hover if a guard is selected
        if (m_game_state.selected_guard.has_value()) {
            for (size_t i = 0; i < m_game_state.caravan_slots.size(); i++) {
                if (!m_game_state.caravan_slots[i].is_visible()) {
                    continue;
                }
                m_game_state.caravan_slots[i].set_highlighted(
                    m_game_state.caravan_slots[i].is_point_inside(cursor_world_point));
            }
        }

        const auto &selected_guard = m_game_state.selected_guard;
        // Highlight the guard the cursor is hovering over
        for (size_t i = 0; i < m_game_state.guards.size(); i++) {
            entity::Entity &guard = m_game_state.guards[i];
            // If a guard is selected, we keep it highlighted
            if (selected_guard.has_value() && selected_guard.value() == i) {
                continue;
            }
            guard.set_highlighted(guard.is_point_inside(cursor_world_point));
        }

        m_quad_renderer->sync_render_slots();
        m_geom_renderer->sync_render_slots();
        m_ui_geom_renderer->sync_render_slots();
        m_ui_text_renderer->sync_render_slots();

        math::Matrix push_constant = m_game_state.camera.get_view_projection_matrix();
        const size_t num_instances = 256; // Size of instance buffers

        auto command_buffer = m_command_buffer_manager->get_command_buffer();

        vulkan::Frame frame = m_swap_chain->begin_frame(command_buffer);

        frame.begin_render_pass(&m_world_render_pass);
        m_quad_renderer->render(command_buffer, &push_constant, num_instances);
        m_geom_renderer->render(command_buffer, &push_constant, num_instances);
        frame.end_render_pass();

        const math::Matrix ui_push_constant = math::Matrix();
        frame.begin_render_pass(&m_ui_render_pass);

        if (m_game_state.event.has_value()) {
            m_ui_geom_renderer->render(command_buffer, &ui_push_constant, 256);
            m_game_state.event->render_text(command_buffer, &ui_push_constant);
        }
        frame.end_render_pass();

        frame.submit_present();

        m_ui_text_renderer->rotate_descriptors();
    }
};
