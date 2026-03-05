#include "Entity.h"
#include "camera/Camera.h"
#include "game_engine_sdk/Game.h"
#include "game_engine_sdk/GameEngine.h"
#include "graphics_pipeline/quad/QuadRenderer.h"
#include "graphics_pipeline/text/TextRenderer.h"
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

// The problem:
// - The problem is that when I define UI elements in the World Coordinate systems, it
// becomes tricky to handle effects such as on_hover() or on_click() events for the UI. I
// want to have the ability to decide wether I should render to the World Coordinate
// system or the Viewport Coordinate system.
// Suggestion 1: Simply create another renderer that renders directly to the screen
// Suggestion 2: Use the same approach as in TextRenderer an issue single draw commands in
// a loop. Then I can pass in which handles I want to draw and with that pass in the push
// constant to render on the viewport.
// Suggestion 3: Make it possible for a render pipeline to have multiple descriptor sets,
// swap descriptor set when I can to render to the viewport vs world.
// ######################################################################################
// TODO: Currently the swap chain clears the existing framebuffer when creating a new
// render pass. THis can't happen if I want multple render_passes.

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
    std::unique_ptr<vulkan::SwapChain> m_swap_chain;

    std::unique_ptr<vulkan::CommandBufferManager> m_command_buffer_manager;

    camera::Camera2D m_camera;
    struct {
        window::ViewportPoint cursor_viewport_position = window::ViewportPoint(1e6, 1e6);
        bool is_right_button_pressed = false;
    } m_mouse_state;

    // World renderers
    vulkan::RenderPass m_render_pass;
    std::unique_ptr<graphics_pipeline::quad::QuadRenderer> m_quad_renderer;
    std::unique_ptr<graphics_pipeline::geometry::GeometryRenderer> m_geom_renderer;
    std::unique_ptr<graphics_pipeline::text::TextRenderer> m_text_renderer;

    // UI renderers
    vulkan::RenderPass m_ui_render_pass;
    std::unique_ptr<graphics_pipeline::geometry::GeometryRenderer> m_ui_geom_renderer;

    std::vector<entity::Entity> m_caravan;
    std::vector<entity::Entity> m_caravan_slots;
    std::vector<entity::Entity> m_guards;
    std::vector<entity::Entity> m_attacks;
    std::vector<entity::Entity> m_enemies;

    struct Event {
        graphics_pipeline::geometry::GeometryRenderer *m_geometry_renderer = nullptr;
        graphics_pipeline::geometry::GeometrySBOHandle m_geometry_render_data;

        graphics_pipeline::text::TextRenderer *m_text_renderer = nullptr;
        graphics_pipeline::text::TextHandle m_event_description;
        std::vector<graphics_pipeline::text::TextHandle> m_event_options;

        template <typename PushConstantType>
        void render(const vulkan::CommandBuffer &command_buffer,
                    PushConstantType *push_constant) {
            DEBUG_ASSERT(m_text_renderer != nullptr,
                         "Error: Attempted to render event text with non-existing "
                         "pointer to a text renderer.");

            m_text_renderer->render(command_buffer, m_event_description, push_constant);
            for (const auto &option : m_event_options) {
                m_text_renderer->render(command_buffer, option, push_constant);
            }
        }

        void remove_event() {
            if (m_geometry_renderer != nullptr) {
                m_geometry_renderer->return_render_slot(m_geometry_render_data);
                m_geometry_renderer = nullptr;
            }

            if (m_text_renderer != nullptr) {
                m_text_renderer->remove_text(std::move(m_event_description));
                for (auto &option : m_event_options) {
                    m_text_renderer->remove_text(std::move(option));
                }
                m_text_renderer = nullptr;
            }
        }

        // CONTINUE:
        void on_click() {
            // Given a point that if it overlaps with the location of the action of an
            // event, perform that given action and its side effect
        }

        void on_hover(const camera::WorldPoint2D &point) {
            // Given a point that if it overlaps with the location of the action of an
            // event, then highlight that event.
            for (auto &option : m_event_options) {
                auto &instance = m_text_renderer->get_text_format_instance(option);
                if (option.is_point_inside(point)) {
                    instance.font_color = util::colors::YELLOW;
                    return;
                }
                instance.font_color = util::colors::WHITE;
            }
        }

        void on_exit() {}
    };

    std::optional<Event> m_event;

    struct {
        GameState last_state = GameState::Playing;
        GameState state = GameState::Playing;

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

        m_render_pass = m_swap_chain->create_render_pass(world_pass);
        m_ui_render_pass = m_swap_chain->create_render_pass(ui_pass);

        m_command_buffer_manager = std::make_unique<vulkan::CommandBufferManager>(ctx, 2);

        m_camera = camera::Camera2D(window_size.width, window_size.height);
        m_camera.set_zoom(0.1f);

        // World renderers
        vulkan::PushConstantRange push_constant_range;
        push_constant_range.offset = 0;
        push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        push_constant_range.size = camera::Camera2D::matrix_size();

        graphics_pipeline::RendererOpts renderer_opts{};
        renderer_opts.push_constant_range = push_constant_range;
        renderer_opts.swap_chain.extent = m_swap_chain->get_extent();
        renderer_opts.swap_chain.render_pass = &m_render_pass;
        renderer_opts.quad.texture = graphics_pipeline::Texture::from_filepath(
            ctx, m_command_buffer_manager.get(), ASSET_FILE("sprite_sheet.png"));
        m_quad_renderer = std::make_unique<graphics_pipeline::quad::QuadRenderer>(
            ctx, m_command_buffer_manager.get(), renderer_opts);

        m_geom_renderer = std::make_unique<graphics_pipeline::geometry::GeometryRenderer>(
            ctx, m_command_buffer_manager.get(), renderer_opts);

        m_text_renderer =
            std::make_unique<graphics_pipeline::text::TextRenderer>(ctx, renderer_opts);
        m_text_renderer->load_font(
            m_command_buffer_manager.get(),
            font::FontLoader(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf")));

        // UI renderers
        graphics_pipeline::RendererOpts ui_renderer_opts{};
        ui_renderer_opts.push_constant_range = push_constant_range;
        ui_renderer_opts.swap_chain.extent = m_swap_chain->get_extent();
        ui_renderer_opts.swap_chain.render_pass = &m_ui_render_pass;
        m_ui_geom_renderer =
            std::make_unique<graphics_pipeline::geometry::GeometryRenderer>(
                ctx, m_command_buffer_manager.get(), ui_renderer_opts);

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

        m_quad_renderer->sync_render_slots();
        m_geom_renderer->sync_render_slots();

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
            if (m_game_state.last_state == GameState::Event && m_event.has_value()) {
                m_event->remove_event();
                m_event = std::nullopt;
            }

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
            if (m_game_state.last_state == GameState::Playing && !m_event.has_value()) {
                m_event =
                    create_event(m_geom_renderer.get(), m_text_renderer.get(), m_camera);
            }
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
        Event event{};
        event.m_geometry_renderer = geom_renderer;
        event.m_text_renderer = text_renderer;

        event.m_geometry_render_data = event.m_geometry_renderer->request_render_slot();

        const camera::WorldPoint2D event_box_center = camera.get_position();
        const float zoom = camera.get_zoom();

        auto &event_box_instance =
            event.m_geometry_renderer->get_instance(event.m_geometry_render_data);

        const float viewport_font_size = 4 / zoom;
        const float viewport_event_box_width = 200.0f / zoom;
        const float viewport_event_box_height = 120.0f / zoom;
        const float viewport_event_box_border_width = 2.0f / zoom;
        const float viewport_event_box_border_radius = 5.0f / zoom;
        const math::Vector2 viewport_event_box_content_padding =
            math::Vector2(5.0f / zoom, (5.0f / zoom) + viewport_font_size / 2.0f);

        event_box_instance.model_matrix =
            math::Matrix()
                .translate(event_box_center)
                .scale(viewport_event_box_width, viewport_event_box_height);
        event_box_instance.color = util::colors::rgba(0.0f, 0.0f, 0.0f, 0.95f);
        event_box_instance.flags |=
            static_cast<uint32_t>(graphics_pipeline::geometry::GeometryShape::Rectangle);
        event_box_instance.border.color = util::colors::WHITE;
        event_box_instance.border.width = viewport_event_box_border_width;
        event_box_instance.border.radius = viewport_event_box_border_radius;

        const math::Vector2 viewport_text_start =
            math::Vector2(event_box_center.x - viewport_event_box_width / 2.0f,
                          event_box_center.y - viewport_event_box_height / 2.0f) +
            viewport_event_box_content_padding;
        event.m_event_description = event.m_text_renderer->create_text(
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod "
            "tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim "
            "veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea "
            "commodo consequat. Duis aute irure dolor in reprehenderit in voluptate "
            "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint "
            "occaecat "
            "cupidatat non proident, sunt in culpa qui officia deserunt mollit anim "
            "id "
            "est laborum.",
            graphics_pipeline::text::TextOpts{
                .position = viewport_text_start,
                .font_color = util::colors::WHITE,
                .font_size = viewport_font_size,
                .line_width = viewport_event_box_width -
                              2.0f * viewport_event_box_content_padding.x(),
                .line_height = viewport_font_size,
            });

        math::Vector2 offset =
            math::Vector2(0.0f, viewport_event_box_height - 3 * viewport_font_size -
                                    viewport_event_box_content_padding.y());
        event.m_event_options.push_back(event.m_text_renderer->create_text(
            "1. Yes.", graphics_pipeline::text::TextOpts{
                           .position = viewport_text_start + offset,
                           .font_color = util::colors::WHITE,
                           .font_size = viewport_font_size,
                           .line_width = viewport_event_box_width -
                                         2.0f * viewport_event_box_content_padding.x(),
                           .line_height = viewport_font_size,
                       }));

        offset = math::Vector2(0.0f, viewport_event_box_height - 2 * viewport_font_size -
                                         viewport_event_box_content_padding.y());
        event.m_event_options.push_back(event.m_text_renderer->create_text(
            "2. Yes again.",
            graphics_pipeline::text::TextOpts{
                .position = viewport_text_start + offset,
                .font_color = util::colors::WHITE,
                .font_size = viewport_font_size,
                .line_width = viewport_event_box_width -
                              2.0f * viewport_event_box_content_padding.x(),
                .line_height = viewport_font_size,
            }));

        event.m_text_renderer->sync_render_slots();
        event.m_geometry_renderer->sync_render_slots();

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

        math::Matrix push_constant = m_camera.get_view_projection_matrix();
        const size_t num_instances = 256; // Size of instance buffers

        auto command_buffer = m_command_buffer_manager->get_command_buffer();

        vulkan::Frame frame = m_swap_chain->begin_frame(command_buffer);

        frame.begin_render_pass(&m_render_pass);
        m_quad_renderer->render(command_buffer, &push_constant, num_instances);
        m_geom_renderer->render(command_buffer, &push_constant, num_instances);
        if (m_game_state.state == GameState::Event && m_event.has_value()) {
            m_event->on_hover(cursor_world_point);
            m_event->render(command_buffer, &push_constant);
        }
        frame.end_render_pass();

        const math::Matrix ui_push_constant = math::Matrix();
        frame.begin_render_pass(&m_ui_render_pass);
        m_ui_geom_renderer->render(command_buffer, &ui_push_constant, 0);
        frame.end_render_pass();

        frame.submit_present();
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
                    if (m_game_state.state == GameState::Playing &&
                        m_mouse_state.is_right_button_pressed) {
                        camera::WorldPoint2D world_delta =
                            m_camera.viewport_delta_to_world(
                                point - m_mouse_state.cursor_viewport_position);
                        m_camera.set_relative_position(world_delta * INVERT_AXISES);
                    }
                    m_mouse_state.cursor_viewport_position = point;
                    break;
                case window::MouseEvent::SCROLL:
                    if (m_game_state.state == GameState::Playing) {
                        m_camera.set_relative_zoom(point.y * ZOOM_SCALE_FACTOR);
                    }
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

    void register_keyboard_event_handler(vulkan::context::GraphicsContext *ctx) {
        ctx->window->register_keyboard_event_callback(
            [this](window::KeyEvent &key, window::KeyState &state) -> void {
                if (state != window::KeyState::DOWN) {
                    return;
                }

                switch (key) {
                case window::KeyEvent::Y: {
                    if (m_game_state.state == GameState::Playing) {
                        m_game_state.state = GameState::Event;
                    } else {
                        m_game_state.state = GameState::Playing;
                    }

                    break;
                }

                default:
                    break;
                };
            });
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
