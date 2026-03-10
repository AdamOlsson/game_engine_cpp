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
#include "interface/NDCPosition.h"
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

class CaravanDefence : public Game {
  private:
    friend class Configuration;

    std::unique_ptr<vulkan::SwapChain> m_swap_chain;

    std::unique_ptr<vulkan::CommandBufferManager> m_command_buffer_manager;

    camera::Camera2D m_camera;
    struct {
        interface::ViewportPoint cursor_viewport_position =
            interface::ViewportPoint(1e6, 1e6);
        bool is_right_button_pressed = false;
    } m_mouse_state;

    // World renderers
    vulkan::RenderPass m_world_render_pass;
    std::unique_ptr<graphics_pipeline::quad::QuadRenderer> m_quad_renderer;
    std::unique_ptr<graphics_pipeline::geometry::GeometryRenderer> m_geom_renderer;

    // UI renderers
    vulkan::RenderPass m_ui_render_pass;
    std::unique_ptr<graphics_pipeline::text::TextRenderer> m_ui_text_renderer;
    std::unique_ptr<graphics_pipeline::geometry::GeometryRenderer> m_ui_geom_renderer;

    struct GameState m_game_state;

  public:
    CaravanDefence() {}

    ~CaravanDefence() {};

    void setup(std::shared_ptr<vulkan::context::GraphicsContext> &ctx) override {

        auto window_size = ctx->window->get_framebuffer_size<float>();
        m_swap_chain = std::make_unique<vulkan::SwapChain>(ctx);

        m_world_render_pass = m_swap_chain->create_render_pass(world_pass);
        m_ui_render_pass = m_swap_chain->create_render_pass(ui_pass);

        m_command_buffer_manager = std::make_unique<vulkan::CommandBufferManager>(ctx, 2);

        m_camera = camera::Camera2D(window_size.width, window_size.height);
        m_camera.set_zoom(0.1f);

        // renderers
        Configuration::setup_world_renderers(ctx, *this);
        Configuration::setup_ui_renderers(ctx, *this);

        Configuration::setup_initial_game_state(m_geom_renderer.get(),
                                                m_quad_renderer.get(), m_game_state);

        Configuration::setup_mouse_event_handler(ctx, *this);
        Configuration::setup_keyboard_event_handler(ctx, *this);
    }

    void spawn_group_of_enemies() {
        const size_t num_enemies = m_game_state.rng.uniform(1, 3);
        const float group_distance = m_game_state.rng.uniform(800.0f, 1200.0f);
        const float group_angle = m_game_state.rng.uniform(-45.0f, 225.0f);
        const math::Vector2 group_center =
            math::Vector2(group_distance, 0.0f).rotate_z(group_angle);

        const int group_radius = 200;
        for (size_t i = 0; i < num_enemies; i++) {
            const float distance_from_group_center =
                m_game_state.rng.uniform(-group_radius, group_radius);
            const float angle_from_group_center = m_game_state.rng.uniform(0, 360);

            const math::Vector2 enemy_position =
                math::Vector2(distance_from_group_center, 0.0f)
                    .rotate_z(angle_from_group_center);

            const uint32_t enemy_type = m_game_state.rng.uniform(0, 1);
            spawn_enemy(group_center + enemy_position,
                        static_cast<entity::EnemyType>(enemy_type));
        }
    }

    void spawn_enemy(const math::Vector2 &position, const entity::EnemyType type) {
        m_game_state.enemies.push_back(entity::Entity::create_enemy(position));

        const size_t caravan_cart_id = m_game_state.rng.uniform(0, 1);
        m_game_state.enemies.back().set_move_target(
            m_game_state.caravan[caravan_cart_id].get_world_position());
        entity::set_enemy_type(m_game_state.enemies.back(), type);
        m_game_state.enemies.back().set_render_data(m_quad_renderer.get(),
                                                    m_geom_renderer.get());
    }

    void create_attack(entity::Entity &guard, entity::Entity &enemy) {
        m_game_state.attacks.push_back(entity::attack(guard, enemy));
        m_game_state.attacks.back().set_render_data(m_quad_renderer.get(),
                                                    m_geom_renderer.get());
    }

    template <typename T> void update_all(const float dt, std::vector<T> &vec) {
        for (T &t : vec) {
            t.update(dt);
        }
    }

    void update(const float dt) override {
        switch (m_game_state.mode) {
        case GameMode::Playing: {
            update_all(dt, m_game_state.caravan);
            update_all(dt, m_game_state.caravan_slots);
            update_all(dt, m_game_state.attacks);
            update_all(dt, m_game_state.enemies);
            update_all(dt, m_game_state.guards);

            for (int i = m_game_state.attacks.size() - 1; i >= 0; i--) {
                if (!m_game_state.attacks[i].is_visible()) {
                    m_game_state.attacks[i].clear_render_data();
                    m_game_state.attacks.erase(m_game_state.attacks.begin() + i);
                }
            }

            const size_t time_elapsed_ms = m_game_state.time_elapsed_ms;
            if (time_elapsed_ms > entity::enemy_t::spawn_rate_ms) {
                spawn_group_of_enemies();
                m_game_state.time_elapsed_ms = 0;
            }

            for (size_t i = 0; i < m_game_state.enemies.size(); i++) {
                entity::Entity &enemy = m_game_state.enemies[i];

                for (entity::Entity &guard : m_game_state.guards) {
                    if (enemy.is_alive() && entity::can_attack(guard) &&
                        entity::in_attack_range(guard, enemy)) {
                        create_attack(guard, enemy);
                        break;
                    }
                }

                // Find the id of the cart the enemy is inside.
                auto cart_id = find_caravan_cart(enemy.get_world_position());
                if (enemy.is_alive() && cart_id.has_value()) {
                    m_game_state.caravan[cart_id.value()].damage(
                        enemy.get_current_health());
                    enemy.kill();
                }

                if (enemy.is_dead()) {
                    enemy.clear_render_data();
                    m_game_state.enemies.erase(m_game_state.enemies.begin() + i);
                }
            }

            for (auto &cart : m_game_state.caravan) {
                if (cart.is_dead()) {
                    logger::info("Cart died, game over!");
                    exit(0);
                }
            }

            m_game_state.time_elapsed_ms += dt * 1000;
            break;
        }

        case GameMode::Event: {
            const interface::NDCPoint cursor_ndc_point =
                m_camera.to_ndc_point(m_mouse_state.cursor_viewport_position);
            m_game_state.event->on_hover(cursor_ndc_point);
            break;
        }

        case GameMode::Paused: {
            break;
        }

        default:
            break;
        };

        m_game_state.last_mode = m_game_state.mode;
    };

    Event create_event() {
        return EventFactory::my_first_event(
            m_ui_geom_renderer.get(), m_ui_text_renderer.get(), m_camera, m_game_state);
    }

    std::optional<size_t> find_caravan_cart(const camera::WorldPoint2D &point) {
        for (size_t i = 0; i < m_game_state.caravan.size(); i++) {
            if (m_game_state.caravan[i].is_point_inside(point)) {
                return i;
            }
        }
        return std::nullopt;
    }

    std::optional<size_t>
    find_selected_caravan_slot(const camera::WorldPoint2D &click_point) {
        for (size_t i = 0; i < m_game_state.caravan_slots.size(); i++) {
            if (m_game_state.caravan_slots[i].is_point_inside(click_point)) {
                return i;
            }
        }
        return std::nullopt;
    }

    std::optional<size_t> find_selected_guard(const camera::WorldPoint2D &click_point) {
        for (size_t i = 0; i < m_game_state.guards.size(); i++) {
            if (m_game_state.guards[i].is_point_inside(click_point)) {
                return i;
            }
        }
        return std::nullopt;
    }

    void render() override {

        const camera::WorldPoint2D cursor_world_point =
            m_camera.viewport_to_world(m_mouse_state.cursor_viewport_position);

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

        math::Matrix push_constant = m_camera.get_view_projection_matrix();
        const size_t num_instances = 256; // Size of instance buffers

        auto command_buffer = m_command_buffer_manager->get_command_buffer();

        vulkan::Frame frame = m_swap_chain->begin_frame(command_buffer);

        frame.begin_render_pass(&m_world_render_pass);
        m_quad_renderer->render(command_buffer, &push_constant, num_instances);
        m_geom_renderer->render(command_buffer, &push_constant, num_instances);
        frame.end_render_pass();

        const math::Matrix ui_push_constant = math::Matrix();
        frame.begin_render_pass(&m_ui_render_pass);

        if (m_game_state.mode == GameMode::Event && m_game_state.event.has_value()) {
            m_ui_geom_renderer->render(command_buffer, &ui_push_constant, 256);
            m_game_state.event->render_text(command_buffer, &ui_push_constant);
        }
        frame.end_render_pass();

        frame.submit_present();

        m_ui_text_renderer->rotate_descriptors();
    }
};
