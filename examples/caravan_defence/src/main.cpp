#include "Entity.h"
#include "camera/Camera.h"
#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "graphics_pipeline/quad/QuadRenderer.h"
#include "vulkan/CommandBufferManager.h"
#include "vulkan/SwapChainManager.h"
#include <random>

#define ASSET_FILE(filename) ASSET_DIR "/" filename
constexpr glm::vec2 INVERT_AXISES = glm::vec2(-1.0f, -1.0f);
constexpr float ZOOM_SCALE_FACTOR = 0.1f;

class CaravanDefence : public Game {
  private:
    std::unique_ptr<vulkan::SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<vulkan::CommandBufferManager> m_command_buffer_manager;

    camera::Camera2D m_camera;
    struct {
        window::ViewportPoint cursor_viewport_position = window::ViewportPoint(1e6, 1e6);
        bool is_right_button_pressed = false;
    } m_mouse_state;

    std::unique_ptr<graphics_pipeline::quad::QuadRenderer> m_quad_renderer;
    std::unique_ptr<graphics_pipeline::geometry::GeometryRenderer> m_geom_renderer;

    std::vector<entity::Entity> m_caravan;
    std::vector<entity::Entity> m_caravan_slots;
    std::vector<entity::Entity> m_guards;
    std::vector<entity::Entity> m_attacks;
    std::vector<entity::Entity> m_enemies;

    struct {
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
        m_swap_chain_manager = std::make_unique<vulkan::SwapChainManager>(ctx);
        m_command_buffer_manager = std::make_unique<vulkan::CommandBufferManager>(ctx, 2);

        m_camera = camera::Camera2D(window_size.width, window_size.height);
        m_camera.set_zoom(0.1f);

        vulkan::PushConstantRange push_constant_range;
        push_constant_range.offset = 0;
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        push_constant_range.size = camera::Camera2D::matrix_size();

        graphics_pipeline::quad::QuadRendererOpts quad_opts{};
        quad_opts.texture = graphics_pipeline::Texture::from_filepath(
            ctx, m_command_buffer_manager.get(), ASSET_FILE("sprite_sheet.png"));
        m_quad_renderer = std::make_unique<graphics_pipeline::quad::QuadRenderer>(
            ctx, m_command_buffer_manager.get(), m_swap_chain_manager.get(),
            &push_constant_range, std::move(quad_opts));

        graphics_pipeline::geometry::GeometryRendererOpts geom_opts{};
        m_geom_renderer = std::make_unique<graphics_pipeline::geometry::GeometryRenderer>(
            ctx, m_command_buffer_manager.get(), m_swap_chain_manager.get(),
            &push_constant_range, std::move(geom_opts));

        // Add entities
        m_caravan.push_back(
            entity::Entity::create_caravan_cart(camera::WorldPoint2D(0.0f, 0.0f)));
        m_caravan.back().set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
        m_caravan.back().set_uvwt(0.3f, 0.0f, 0.4f, 0.2f);

        m_caravan.push_back(
            entity::Entity::create_caravan_cart(camera::WorldPoint2D(0.0f, 275.0f)));
        m_caravan.back().set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
        m_caravan.back().set_uvwt(0.2f, 0.0f, 0.3f, 0.2f);

        m_caravan_slots.push_back(
            entity::Entity::create_caravan_slot(camera::WorldPoint2D(-200.0f, -50.0f)));
        m_caravan_slots.push_back(
            entity::Entity::create_caravan_slot(camera::WorldPoint2D(200.0f, -50.0f)));
        m_caravan_slots.push_back(
            entity::Entity::create_caravan_slot(camera::WorldPoint2D(0.0f, -250.0f)));
        m_caravan_slots.push_back(
            entity::Entity::create_caravan_slot(camera::WorldPoint2D(-200.0f, 300.0f)));
        m_caravan_slots.push_back(
            entity::Entity::create_caravan_slot(camera::WorldPoint2D(200.0f, 300.0f)));

        m_caravan_slots[0].set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
        m_caravan_slots[1].set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
        m_caravan_slots[2].set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
        m_caravan_slots[3].set_render_data(m_quad_renderer.get(), m_geom_renderer.get());
        m_caravan_slots[4].set_render_data(m_quad_renderer.get(), m_geom_renderer.get());

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

        m_quad_renderer->sync_render_slots();
        m_geom_renderer->sync_render_slots();

        register_mouse_event_handler(ctx.get());
    }

    void spawn_group_of_enemies() {

        const size_t num_enemies = m_game_state.rng.uniform(1, 3);
        const float group_distance = m_game_state.rng.uniform(600.0f, 1000.0f);
        const float group_angle = m_game_state.rng.uniform(-45.0f, 225.0f);
        const math::Vector2 group_center =
            math::Vector2(group_distance, 0.0f).rotate_z(group_angle);

        const int group_radius = 150;
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
    };

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

    void register_mouse_event_handler(vulkan::context::GraphicsContext *ctx) {
        ctx->window->register_mouse_event_callback(
            [this](window::MouseEvent mouse_event, window::ViewportPoint &point) -> void {
                switch (mouse_event) {
                case window::MouseEvent::RIGHT_BUTTON_DOWN:
                    m_mouse_state.is_right_button_pressed = true;
                    break;
                case window::MouseEvent::RIGHT_BUTTON_UP: {
                    m_mouse_state.is_right_button_pressed = false;
                    break;
                }
                case window::MouseEvent::CURSOR_MOVED:
                    if (m_mouse_state.is_right_button_pressed) {
                        camera::WorldPoint2D world_delta =
                            m_camera.viewport_delta_to_world(
                                point - m_mouse_state.cursor_viewport_position);
                        m_camera.set_relative_position(world_delta * INVERT_AXISES);
                    }
                    m_mouse_state.cursor_viewport_position = point;
                    break;
                case window::MouseEvent::SCROLL:
                    m_camera.set_relative_zoom(point.y * ZOOM_SCALE_FACTOR);
                    break;
                case window::MouseEvent::LEFT_BUTTON_DOWN:
                    break;
                case window::MouseEvent::LEFT_BUTTON_UP: {
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
                    break;
                }
                }
            });
    }

    void render() override {

        auto command_buffer = m_command_buffer_manager->get_command_buffer();
        vulkan::RenderPass render_pass =
            m_swap_chain_manager->get_render_pass(command_buffer);
        render_pass.begin();

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

        glm::mat4 push_constant = m_camera.get_view_projection_matrix();
        const size_t num_instances = 256; // Size of instance buffers
        m_quad_renderer->render(command_buffer, &push_constant, num_instances);
        m_geom_renderer->render(command_buffer, &push_constant, num_instances);

        render_pass.end_submit_present();
    }
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(1080, 960),
                                              .title = "Caravan Defence"},
    };

    auto game = std::make_unique<CaravanDefence>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
