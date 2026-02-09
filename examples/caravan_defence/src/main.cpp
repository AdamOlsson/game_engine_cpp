#include "Caravan.h"
#include "CaravanSlot.h"
#include "Enemy.h"
#include "Guard.h"
#include "RangedAttack.h"
#include "camera/Camera.h"
#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "graphics_pipeline/quad/QuadRenderer.h"
#include "math/Vector2.h"
#include "vulkan/CommandBufferManager.h"
#include "vulkan/SwapChainManager.h"
#include "vulkan/buffers/GpuBuffer.h"
#include <random>

constexpr glm::vec2 INVERT_AXISES = glm::vec2(-1.0f, -1.0f);
constexpr float ZOOM_SCALE_FACTOR = 0.1f;

class CaravanDefence : public Game {
  private:
    std::unique_ptr<vulkan::SwapChainManager> m_swap_chain_manager;
    std::unique_ptr<vulkan::CommandBufferManager> m_command_buffer_manager;

    camera::Camera2D m_camera;
    struct {
        window::ViewportPoint cursor_viewport_position;
        bool is_right_button_pressed = false;
    } m_mouse_state;

    std::unique_ptr<graphics_pipeline::quad::QuadRenderer> m_quad_renderer;

    Caravan m_caravan;
    std::vector<CaravanSlot> m_caravan_slots;
    std::vector<Guard> m_guards;
    std::vector<RangedAttack> m_attacks;
    std::vector<Enemy> m_enemies;

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
        m_quad_renderer = std::make_unique<graphics_pipeline::quad::QuadRenderer>(
            ctx, m_command_buffer_manager.get(), m_swap_chain_manager.get(),
            &push_constant_range, std::move(quad_opts));

        // Add entities
        m_caravan = Caravan(camera::WorldPoint2D(0.0f, 0.0f));
        m_caravan_slots.push_back(CaravanSlot(camera::WorldPoint2D(-150.0f, -50.0f)));
        m_caravan_slots.push_back(CaravanSlot(camera::WorldPoint2D(150.0f, -50.0f)));
        m_caravan_slots.push_back(CaravanSlot(camera::WorldPoint2D(0.0f, -200.0f)));

        m_caravan.set_render_data(&m_quad_renderer->m_instances.emplace_back());
        m_caravan_slots[0].set_render_data(&m_quad_renderer->m_instances.emplace_back());
        m_caravan_slots[1].set_render_data(&m_quad_renderer->m_instances.emplace_back());
        m_caravan_slots[2].set_render_data(&m_quad_renderer->m_instances.emplace_back());

        m_guards.reserve(16); // 16 is magic
        m_guards.push_back(Guard(&m_caravan_slots[0]));
        m_guards.push_back(Guard(&m_caravan_slots[1]));

        m_enemies.reserve(64);

        for (Guard &g : m_guards) {
            g.set_render_data(&m_quad_renderer->m_instances.emplace_back());
        }

        m_quad_renderer->m_instances.sync_all();

        register_mouse_event_handler(ctx.get());
    }

    void spawn_enemy() {
        const float distance = m_game_state.rng.uniform(400.0f, 800.0f);
        const float angle = m_game_state.rng.uniform(-45.0f, 225.0f);
        const math::Vector2 position = math::Vector2(distance, 0.0f).rotate_z(-angle);
        m_enemies.push_back(Enemy(position));
        m_enemies.back().set_render_data(&m_quad_renderer->m_instances.emplace_back());
    }

    void create_attack(Guard &guard, const Enemy &enemy) {
        m_attacks.push_back(guard.attack(enemy.get_world_position()));
        m_attacks.back().set_render_data(&m_quad_renderer->m_instances.emplace_back());
    }

    template <typename T> void update_all(const float dt, std::vector<T> &vec) {
        for (T &t : vec) {
            t.update(dt);
        }
    }

    void update(const float dt) override {
        update_all(dt, m_attacks);

        for (int i = m_attacks.size() - 1; i >= 0; i--) {
            if (!m_attacks[i].is_visible()) {
                m_attacks[i].remove();
                m_attacks.erase(m_attacks.begin() + i);
            }
        }

        const size_t time_elapsed_ms = m_game_state.time_elapsed_ms;
        if (time_elapsed_ms > Enemy::spawn_rate_ms) {
            spawn_enemy();
            m_game_state.time_elapsed_ms = 0;
        }

        for (size_t i = 0; i < m_enemies.size(); i++) {
            bool enemy_dead = false;
            Enemy &enemy = m_enemies[i];
            enemy.move_towards(m_caravan.get_world_position(), dt);
            const camera::WorldPoint2D enemy_position = enemy.get_world_position();

            for (Guard &guard : m_guards) {
                if (!enemy_dead && guard.can_attack() &&
                    guard.in_attack_range(enemy_position)) {
                    create_attack(guard, enemy);
                    enemy_dead = true;
                    break;
                }
            }

            if (!enemy_dead && m_caravan.is_point_inside(enemy.get_world_position())) {
                enemy_dead = true;
            }

            if (enemy_dead) {
                enemy.die();
                m_enemies.erase(m_enemies.begin() + i);
            }
        }

        m_game_state.time_elapsed_ms += dt * 1000;
    };

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

                    if (m_game_state.selected_guard.has_value()) {
                        m_guards[m_game_state.selected_guard.value()].set_selected(false);
                    }

                    const auto new_selected_guard = find_selected_guard(world_point);
                    const auto clicked_caravan_slot =
                        find_selected_caravan_slot(world_point);

                    if (m_game_state.selected_guard.has_value() &&
                        clicked_caravan_slot.has_value()) {
                        CaravanSlot &slot = m_caravan_slots[clicked_caravan_slot.value()];
                        Guard &guard = m_guards[m_game_state.selected_guard.value()];

                        if (slot.is_free()) {
                            CaravanSlot *old_slot = guard.get_caravan_slot();
                            guard.set_caravan_slot(&slot);
                            old_slot->clear_occupying_guard();
                        }
                    }

                    m_game_state.selected_guard = new_selected_guard;

                    if (m_game_state.selected_guard.has_value()) {
                        m_guards[m_game_state.selected_guard.value()].set_selected(true);
                    }

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

        // Only highlight slots when hover if a guard is selected
        if (m_game_state.selected_guard.has_value()) {
            const camera::WorldPoint2D cursor_world_point =
                m_camera.viewport_to_world(m_mouse_state.cursor_viewport_position);
            for (size_t i = 0; i < m_caravan_slots.size(); i++) {
                if (!m_caravan_slots[i].is_visible()) {
                    continue;
                }

                if (m_caravan_slots[i].is_point_inside(cursor_world_point)) {
                    m_caravan_slots[i].set_highlighted(true);
                } else {
                    m_caravan_slots[i].set_highlighted(false);
                }
            }
        }

        m_quad_renderer->m_instances.sync();

        glm::mat4 push_constant = m_camera.get_view_projection_matrix();
        /*const size_t num_instances = 1 + m_guards.size() + m_caravan_slots.size() +*/
        /*                             m_enemies.size() + m_attacks.size();*/
        const size_t num_instances = 256;
        m_quad_renderer->render(command_buffer, &push_constant, num_instances);

        render_pass.end_submit_present();
    }
};

int main() {

    GameEngineConfig config{
        .window_config = window::WindowConfig{.dims = window::WindowDimension(800, 800),
                                              .title = "Caravan Defence"},
    };

    auto game = std::make_unique<CaravanDefence>();
    auto game_engine = std::make_unique<GameEngine>(std::move(game), config);

    game_engine->run();

    return 0;
}
