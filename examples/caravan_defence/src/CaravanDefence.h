#pragma once

#include "Configuration.h"
#include "EventFactory.h"
#include "GameState.h"
#include "camera/Camera.h"
#include "game_engine_sdk/Game.h"
#include "graphics_pipeline/geometry/GeometryRenderer2.h"
#include "graphics_pipeline/quad/QuadRenderer2.h"
#include "states/IntroState.h"
#include "states/state_machine/StateMachine.h"
#include "states/types.h"
#include "vulkan/CommandBufferManager.h"
#include "vulkan/SwapChain.h"

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

class CaravanDefence : public Game {
  private:
    friend class Configuration;

    std::unique_ptr<vulkan::SwapChain> m_swap_chain;

    std::unique_ptr<vulkan::CommandBufferManager> m_command_buffer_manager;

    // World renderers
    vulkan::RenderPass m_world_render_pass;
    std::unique_ptr<graphics_pipeline::quad::QuadRenderer2> m_world_quad_renderer;
    std::unique_ptr<graphics_pipeline::geometry::GeometryRenderer2>
        m_world_geom_renderer2;

    // UI renderers
    vulkan::RenderPass m_ui_render_pass;
    std::unique_ptr<graphics_pipeline::text::TextRenderer2> m_ui_text_renderer2;
    std::unique_ptr<graphics_pipeline::geometry::GeometryRenderer2> m_ui_geom_renderer2;

    size_t m_map_tiles_count = 0;
    std::vector<vulkan::DrawIndexedIndirectCommand> m_map_layer_1_chunk_draw_commands;
    std::vector<vulkan::DrawIndexedIndirectCommand> m_map_layer_2_chunk_draw_commands;

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
        m_game_state.camera.configure_max_zoom(2.0f);
        m_game_state.camera.configure_min_zoom(0.05f);
        m_game_state.camera.set_zoom(0.1f);

        // renderers
        Configuration::setup_world_renderers(ctx, *this);
        Configuration::setup_ui_renderers(ctx, *this);

        Configuration::setup_mouse_event_handler(ctx, *this);
        Configuration::setup_keyboard_event_handler(ctx, *this);

        Configuration::setup_initial_game_state(m_game_state);

        m_state_machine.get_state<IntroState>() = IntroState(m_ui_text_renderer2->m_font);
        m_state_machine.get_state<DefendState>() =
            DefendState(m_ui_text_renderer2->m_font);
        m_state_machine.get_state<EventState>() =
            EventState(m_ui_text_renderer2.get(), m_ui_geom_renderer2.get());

        m_state_machine.init<IntroState>();
    }

    void update(const float dt) override { m_state_machine.update(dt, m_game_state); };

    void render_world(vulkan::CommandBuffer &command_buffer, vulkan::Frame &frame) {

        math::Matrix push_constant = m_game_state.camera.get_view_projection_matrix();

        frame.begin_render_pass(&m_world_render_pass);

        m_world_quad_renderer->write_to_draw_command_buffer(
            m_map_layer_1_chunk_draw_commands, 0);
        m_world_quad_renderer->write_to_draw_command_buffer(
            m_map_layer_2_chunk_draw_commands, m_map_layer_1_chunk_draw_commands.size());
        const size_t map_tile_draw_commands_count =
            m_map_layer_1_chunk_draw_commands.size() +
            m_map_layer_2_chunk_draw_commands.size();

        auto &defend_state = m_state_machine.get_state<DefendState>();
        const auto quad_instance_data = defend_state.get_quad_render_data(m_game_state);

        const auto quad_instance_data_render_command =
            m_world_quad_renderer->write_to_buffer(quad_instance_data, m_map_tiles_count);
        m_world_quad_renderer->write_to_draw_command_buffer(
            {quad_instance_data_render_command}, map_tile_draw_commands_count);

        // quad: render world map tiles first layer
        m_world_quad_renderer->render_indirect(
            command_buffer, &push_constant, m_map_layer_1_chunk_draw_commands.size(), 0);

        // quad: render world map tiles second layer
        m_world_quad_renderer->render_indirect(command_buffer, &push_constant,
                                               m_map_layer_2_chunk_draw_commands.size(),
                                               m_map_layer_1_chunk_draw_commands.size());

        // quad: render entity quad data
        m_world_quad_renderer->render_indirect(command_buffer, &push_constant, 1,
                                               map_tile_draw_commands_count);

        const auto geom_instance_data =
            defend_state.get_geometry_render_data(m_game_state);
        vulkan::DrawIndexedIndirectCommand geometry_instance_data_draw_command =
            m_world_geom_renderer2->write_to_buffer({geom_instance_data});

        m_world_geom_renderer2->write_to_draw_command_buffer(
            {geometry_instance_data_draw_command});

        // geometry: render entity geometry data (attack range, healthbars etc)
        m_world_geom_renderer2->render_indirect(command_buffer, &push_constant, 1, 0);

        frame.end_render_pass();
    }

    void render_ui(vulkan::CommandBuffer &command_buffer, vulkan::Frame &frame) {
        const math::Matrix ui_push_constant = math::Matrix();
        frame.begin_render_pass(&m_ui_render_pass);

        std::vector<font::TextFormat> text_format;
        std::vector<font::Text> text;
        std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> geometry_data;

        if (m_state_machine.is_in_state<EventState>()) {
            auto &state = m_state_machine.get_state<EventState>();
            text_format = state.get_ui_text_format_render_data();
            text = state.get_ui_text_render_data();
            geometry_data = state.get_ui_geometry_render_data();
        } else if (m_state_machine.is_in_state<IntroState>()) {
            auto &state = m_state_machine.get_state<IntroState>();
            text_format = state.get_ui_text_format_render_data();
            text = state.get_ui_text_render_data();
            geometry_data = state.get_ui_geometry_render_data();
        } else if (m_state_machine.is_in_state<DefendState>()) {
            auto &state = m_state_machine.get_state<DefendState>();
            text_format = state.get_ui_text_format_render_data();
            text = state.get_ui_text_render_data();
            geometry_data = state.get_ui_geometry_render_data();
        }

        vulkan::DrawIndexedIndirectCommand geom_draw_command =
            m_ui_geom_renderer2->write_to_buffer(geometry_data);
        m_ui_geom_renderer2->write_to_draw_command_buffer({geom_draw_command});

        // UI: render the UI geometry data (background bounding boxes etc)
        m_ui_geom_renderer2->render_indirect(command_buffer, &ui_push_constant, 1, 0);

        size_t offset = 0;
        std::vector<vulkan::DrawIndexedIndirectCommand> text_draw_commands;
        for (size_t i = 0; i < text.size(); i++) {
            size_t format_idx = i;
            m_ui_text_renderer2->write_to_format_buffer({text_format[i]}, format_idx);
            auto draw_cmd =
                m_ui_text_renderer2->write_to_glyph_buffer(text[i], format_idx, offset);
            offset += text[i].glyphs.size();

            text_draw_commands.insert(text_draw_commands.end(), draw_cmd.begin(),
                                      draw_cmd.end());
        }

        m_ui_text_renderer2->render_indirect(command_buffer, &ui_push_constant,
                                             text_draw_commands);

        frame.end_render_pass();
    }

    void render() override {

        auto command_buffer = m_command_buffer_manager->get_command_buffer();

        vulkan::Frame frame = m_swap_chain->begin_frame(command_buffer);

        render_world(command_buffer, frame);
        render_ui(command_buffer, frame);

        frame.submit_present();
    }
};
