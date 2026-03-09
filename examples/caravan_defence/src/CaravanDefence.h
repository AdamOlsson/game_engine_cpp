#pragma once

#include "Configuration.h"
#include "Entity.h"
#include "camera/Camera.h"
#include "events/DialogFactory.h"
#include "events/Event.h"
#include "game_engine_sdk/Game.h"
#include "graphics_pipeline/quad/QuadRenderer.h"
#include "graphics_pipeline/text/TextRenderer.h"
#include "interface/NDCPosition.h"
#include "vulkan/CommandBufferManager.h"
#include <random>

#define ASSET_FILE(filename) ASSET_DIR "/" filename
constexpr glm::vec2 INVERT_AXISES = glm::vec2(-1.0f, -1.0f);
constexpr float ZOOM_SCALE_FACTOR = 0.1f;

enum class GameState {
    Playing,
    Paused,
    Event,
};

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

// TODO: Move relevant members into the anonymous game state struct. Then pass
// this game state to event construction.
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

    std::vector<entity::Entity> m_caravan;
    std::vector<entity::Entity> m_caravan_slots;
    std::vector<entity::Entity> m_guards;
    std::vector<entity::Entity> m_attacks;
    std::vector<entity::Entity> m_enemies;

    struct {

        GameState last_state = GameState::Playing;
        GameState state = GameState::Playing;

        std::optional<Event> event;

        size_t time_elapsed_ms;
        std::optional<size_t> selected_guard = std::nullopt;

        struct {
            std::random_device device;
            std::mt19937 gen;
            void init() { gen = std::mt19937(device()); }
            float uniform(const float from, const float to) {
                std::uniform_real_distribution<float> dist(from, to);
                return dist(gen);
            }

            int uniform(const int from, const int to) {
                std::uniform_int_distribution<int> dist(from, to);
                return dist(gen);
            }
        } rng;
    } m_game_state;

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

        const float slot_distance_x = 250.0f;
        // Add entities
        m_caravan.push_back(
            entity::Entity::create_caravan_cart(camera::WorldPoint2D(0.0f, 0.0f)));
        m_caravan.back().set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
        m_caravan.back().set_uvwt(0.3f, 0.0f, 0.4f, 0.2f);
        m_caravan_slots.push_back(
            entity::Entity::create_caravan_slot(camera::WorldPoint2D(
                slot_distance_x, m_caravan.back().get_world_position().y)));
        m_caravan_slots.back().set_render_data(m_quad_renderer.get(),
                                               m_geom_renderer.get());
        m_caravan_slots.push_back(
            entity::Entity::create_caravan_slot(camera::WorldPoint2D(
                -slot_distance_x, m_caravan.back().get_world_position().y)));
        m_caravan_slots.back().set_render_data(m_quad_renderer.get(),
                                               m_geom_renderer.get());

        m_caravan.push_back(
            entity::Entity::create_caravan_cart(camera::WorldPoint2D(0.0f, 275.0f)));
        m_caravan.back().set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
        m_caravan.back().set_uvwt(0.2f, 0.0f, 0.3f, 0.2f);
        m_caravan_slots.push_back(
            entity::Entity::create_caravan_slot(camera::WorldPoint2D(
                slot_distance_x, m_caravan.back().get_world_position().y)));
        m_caravan_slots.back().set_render_data(m_quad_renderer.get(),
                                               m_geom_renderer.get());
        m_caravan_slots.push_back(
            entity::Entity::create_caravan_slot(camera::WorldPoint2D(
                -slot_distance_x, m_caravan.back().get_world_position().y)));
        m_caravan_slots.back().set_render_data(m_quad_renderer.get(),
                                               m_geom_renderer.get());

        m_guards.reserve(16); // 16 is magic
        m_guards.push_back(
            entity::Entity::create_guard(camera::WorldPoint2D(0.0f, 0.0f)));
        m_guards.push_back(
            entity::Entity::create_guard(camera::WorldPoint2D(0.0f, 0.0f)));

        set_damage_type(m_guards[0], entity::DamageType::A);
        set_damage_type(m_guards[1], entity::DamageType::B);

        m_enemies.reserve(64);
        m_attacks.reserve(8);

        for (size_t i = 0; i < m_guards.size(); i++) {
            m_guards[i].set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
            entity::set_caravan_slot(m_guards[i], &m_caravan_slots[i]);
            entity::set_occupying_guard(m_caravan_slots[i], &m_guards[i]);
        }

        register_mouse_event_handler(ctx.get());
        register_keyboard_event_handler(ctx.get());
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
        m_enemies.push_back(entity::Entity::create_enemy(position));

        const size_t caravan_cart_id = m_game_state.rng.uniform(0, 1);
        m_enemies.back().set_move_target(m_caravan[caravan_cart_id].get_world_position());
        entity::set_enemy_type(m_enemies.back(), type);
        m_enemies.back().set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
    }

    void create_attack(entity::Entity &guard, entity::Entity &enemy) {
        m_attacks.push_back(entity::attack(guard, enemy));
        m_attacks.back().set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
    }

    template <typename T> void update_all(const float dt, std::vector<T> &vec) {
        for (T &t : vec) {
            t.update(dt);
        }
    }

    void update(const float dt) override {
        switch (m_game_state.state) {
        case GameState::Playing: {
            update_all(dt, m_caravan);
            update_all(dt, m_caravan_slots);
            update_all(dt, m_attacks);
            update_all(dt, m_enemies);
            update_all(dt, m_guards);

            for (int i = m_attacks.size() - 1; i >= 0; i--) {
                if (!m_attacks[i].is_visible()) {
                    m_attacks[i].clear_render_data();
                    m_attacks.erase(m_attacks.begin() + i);
                }
            }

            const size_t time_elapsed_ms = m_game_state.time_elapsed_ms;
            if (time_elapsed_ms > entity::enemy_t::spawn_rate_ms) {
                spawn_group_of_enemies();
                m_game_state.time_elapsed_ms = 0;
            }

            for (size_t i = 0; i < m_enemies.size(); i++) {
                entity::Entity &enemy = m_enemies[i];

                for (entity::Entity &guard : m_guards) {
                    if (enemy.is_alive() && entity::can_attack(guard) &&
                        entity::in_attack_range(guard, enemy)) {
                        create_attack(guard, enemy);
                        break;
                    }
                }

                // Find the id of the cart the enemy is inside.
                auto cart_id = find_caravan_cart(enemy.get_world_position());
                if (enemy.is_alive() && cart_id.has_value()) {
                    m_caravan[cart_id.value()].damage(enemy.get_current_health());
                    enemy.kill();
                }

                if (enemy.is_dead()) {
                    enemy.clear_render_data();
                    m_enemies.erase(m_enemies.begin() + i);
                }
            }

            for (auto &cart : m_caravan) {
                if (cart.is_dead()) {
                    logger::info("Cart died, game over!");
                    exit(0);
                }
            }

            m_game_state.time_elapsed_ms += dt * 1000;
            break;
        }

        case GameState::Event: {
            const interface::NDCPoint cursor_ndc_point =
                m_camera.to_ndc_point(m_mouse_state.cursor_viewport_position);
            m_game_state.event->on_hover(cursor_ndc_point);
            break;
        }

        case GameState::Paused: {
            break;
        }

        default:
            break;
        };

        m_game_state.last_state = m_game_state.state;
    };

    Event create_event(graphics_pipeline::geometry::GeometryRenderer *geom_renderer,
                       graphics_pipeline::text::TextRenderer *text_renderer,
                       const camera::Camera2D &camera) {

        const float dialog_font_size = camera.to_ndc_width(22);
        const math::Vector2 dialog_box_center = interface::NDCPoint(0, 0);
        const math::Vector2 dialog_box_size(1.80f, 1.20f);
        const math::Vector2 dialog_box_content_padding(0.03f, 0.04f);
        const math::Vector2 dialog_box_content_size =
            dialog_box_size - dialog_box_content_padding * 2.0f;

        const math::Vector2 dialog_line_size(dialog_box_content_size.x(),
                                             dialog_font_size);

        const float content_top_y_ndc = dialog_box_center.y() -
                                        dialog_box_size.y() / 2.0f +
                                        dialog_box_content_padding.y() * 2.0f;
        const float content_bottom_y_ndc = dialog_box_center.y() +
                                           dialog_box_size.y() / 2.0f +
                                           dialog_box_content_padding.y();
        const float dialog_text_start_x_ndc = dialog_box_center.x() -
                                              dialog_box_size.x() / 2.0f +
                                              dialog_box_content_padding.x();
        const float text_start_y_ndc = content_top_y_ndc + dialog_line_size.y();
        const float text_start_y_options_ndc =
            content_bottom_y_ndc - dialog_line_size.y() * 3;

        auto event_desc_opts = graphics_pipeline::text::TextOpts{};
        event_desc_opts.position =
            interface::NDCPoint(dialog_text_start_x_ndc, text_start_y_ndc);
        event_desc_opts.font_color = util::colors::WHITE;
        event_desc_opts.font_size = dialog_font_size;
        event_desc_opts.line_width = dialog_line_size.x();
        event_desc_opts.line_height = dialog_line_size.y();

        graphics_pipeline::text::TextOpts event_dialog_option_opts{};
        event_dialog_option_opts.position = interface::NDCPoint(
            dialog_text_start_x_ndc, text_start_y_options_ndc - dialog_line_size.y());
        event_dialog_option_opts.font_color = util::colors::WHITE;
        event_dialog_option_opts.font_size = dialog_font_size;
        event_dialog_option_opts.line_width = dialog_line_size.x();
        event_dialog_option_opts.line_height = dialog_line_size.y();

        const auto quit_event = [this]() {
            m_game_state.state = GameState::Playing;
            m_game_state.event = std::nullopt;
        };

        Event event = Event(geom_renderer, text_renderer);

        event.create_dialog_bbox(camera);

        // Consistent, same every time for all events
        DialogFactory dialog_factory = DialogFactory(text_renderer);
        dialog_factory.set_event_dialog_text_opts(event_desc_opts);
        dialog_factory.set_event_dialog_option_text_opts(event_dialog_option_opts);

        std::string node_a_id = "A";
        dialog_factory.set_id(node_a_id);
        dialog_factory.set_dialog_text(
            "This is a really really really long event description that spans multiple "
            "lines about an interesting event.");
        dialog_factory.add_dialog_option("next0", "Next.", "B", []() {});
        dialog_factory.add_dialog_option("exit0", "Exit the event.", std::nullopt,
                                         quit_event);
        event.add_dialog_node(dialog_factory.build());

        dialog_factory.set_id("B");
        dialog_factory.set_dialog_text("...");
        dialog_factory.add_dialog_option("exit1", "Exit again.", std::nullopt,
                                         quit_event);
        event.add_dialog_node(dialog_factory.build());

        event.set_active_node(node_a_id);

        return event;
    }

    std::optional<size_t> find_caravan_cart(const camera::WorldPoint2D &point) {
        for (size_t i = 0; i < m_caravan.size(); i++) {
            if (m_caravan[i].is_point_inside(point)) {
                return i;
            }
        }
        return std::nullopt;
    }

    std::optional<size_t>
    find_selected_caravan_slot(const camera::WorldPoint2D &click_point) {
        for (size_t i = 0; i < m_caravan_slots.size(); i++) {
            if (m_caravan_slots[i].is_point_inside(click_point)) {
                return i;
            }
        }
        return std::nullopt;
    }

    std::optional<size_t> find_selected_guard(const camera::WorldPoint2D &click_point) {
        for (size_t i = 0; i < m_guards.size(); i++) {
            if (m_guards[i].is_point_inside(click_point)) {
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
            for (size_t i = 0; i < m_caravan_slots.size(); i++) {
                if (!m_caravan_slots[i].is_visible()) {
                    continue;
                }
                m_caravan_slots[i].set_highlighted(
                    m_caravan_slots[i].is_point_inside(cursor_world_point));
            }
        }

        const auto &selected_guard = m_game_state.selected_guard;
        // Highlight the guard the cursor is hovering over
        for (size_t i = 0; i < m_guards.size(); i++) {
            entity::Entity &guard = m_guards[i];
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

        if (m_game_state.state == GameState::Event && m_game_state.event.has_value()) {
            m_ui_geom_renderer->render(command_buffer, &ui_push_constant, 256);
            m_game_state.event->render_text(command_buffer, &ui_push_constant);
        }
        frame.end_render_pass();

        frame.submit_present();

        m_ui_text_renderer->rotate_descriptors();
    }

    void register_mouse_event_handler(vulkan::context::GraphicsContext *ctx) {
        ctx->window->register_mouse_event_callback([this](window::MouseEvent mouse_event,
                                                          interface::ViewportPoint &point)
                                                       -> void {
            switch (mouse_event) {
            case window::MouseEvent::RIGHT_BUTTON_DOWN:
                m_mouse_state.is_right_button_pressed = true;
                break;
            case window::MouseEvent::RIGHT_BUTTON_UP: {
                m_mouse_state.is_right_button_pressed = false;
                break;
            }
            case window::MouseEvent::CURSOR_MOVED:
                if (m_game_state.state == GameState::Playing &&
                    m_mouse_state.is_right_button_pressed) {
                    camera::WorldPoint2D world_delta = m_camera.viewport_delta_to_world(
                        point - m_mouse_state.cursor_viewport_position);
                    m_camera.set_relative_position(world_delta * INVERT_AXISES);
                }
                m_mouse_state.cursor_viewport_position = point;
                break;
            case window::MouseEvent::SCROLL:
                if (m_game_state.state == GameState::Playing) {
                    m_camera.set_relative_zoom(point.y() * ZOOM_SCALE_FACTOR);
                }
                break;
            case window::MouseEvent::LEFT_BUTTON_DOWN:
                break;
            case window::MouseEvent::LEFT_BUTTON_UP: {
                if (m_game_state.state == GameState::Playing) {
                    const camera::WorldPoint2D world_point =
                        m_camera.viewport_to_world(point);
                    const auto &current_selected_guard = m_game_state.selected_guard;

                    // Clear currently selected guard
                    if (current_selected_guard.has_value()) {
                        m_guards[current_selected_guard.value()].set_selected(false);
                        m_guards[current_selected_guard.value()].set_highlighted(false);
                    }

                    const auto new_selected_guard = find_selected_guard(world_point);
                    const auto clicked_caravan_slot =
                        find_selected_caravan_slot(world_point);

                    if (current_selected_guard.has_value() &&
                        clicked_caravan_slot.has_value()) {
                        entity::Entity &slot =
                            m_caravan_slots[clicked_caravan_slot.value()];
                        entity::Entity &guard = m_guards[current_selected_guard.value()];

                        if (entity::is_free(slot)) {
                            entity::Entity *old_slot = entity::get_caravan_slot(guard);
                            entity::set_caravan_slot(guard, &slot);
                            entity::set_occupying_guard(slot, &guard);

                            entity::clear_occupying_guard(*old_slot);
                            old_slot->set_highlighted(false);
                        }
                    }

                    if (new_selected_guard.has_value()) {
                        m_guards[new_selected_guard.value()].set_selected(true);
                        m_guards[new_selected_guard.value()].set_highlighted(true);
                    }

                    m_game_state.selected_guard = new_selected_guard;

                } else if (m_game_state.state == GameState::Event &&
                           m_game_state.event.has_value()) {
                    const interface::NDCPoint cursor_ndc_point =
                        m_camera.to_ndc_point(m_mouse_state.cursor_viewport_position);
                    m_game_state.event->on_click(cursor_ndc_point);
                }
                break;
            }
            }
        });
    }

    void register_keyboard_event_handler(vulkan::context::GraphicsContext *ctx) {
        ctx->window->register_keyboard_event_callback(
            [this](window::KeyEvent &key, window::KeyState &state) -> void {
                if (state != window::KeyState::DOWN) {
                    return;
                }

                switch (key) {
                case window::KeyEvent::Y: {
                    if (m_game_state.state == GameState::Playing) {
                        m_game_state.event = create_event(
                            m_ui_geom_renderer.get(), m_ui_text_renderer.get(), m_camera);
                        m_game_state.state = GameState::Event;
                    }
                    break;
                }

                default:
                    break;
                };
            });
    }
};
