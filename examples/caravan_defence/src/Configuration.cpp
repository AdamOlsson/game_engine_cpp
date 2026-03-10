
#include "Configuration.h"
#include "CaravanDefence.h"

void Configuration::setup_world_renderers(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx, CaravanDefence &game) {

    DEBUG_ASSERT(game.m_swap_chain != nullptr,
                 "Error: Swap chain needs to be initialised before world renderers.");

    DEBUG_ASSERT(
        game.m_command_buffer_manager != nullptr,
        "Error: Command buffer manager needs to be initialised before world renderers.");

    DEBUG_ASSERT(
        game.m_world_render_pass != nullptr,
        "Error: World render pass needs to be initialised before world renderers.");

    // World renderers
    vulkan::PushConstantRange push_constant_range;
    push_constant_range.offset = 0;
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_range.size = camera::Camera2D::matrix_size();

    graphics_pipeline::RendererOpts renderer_opts{};
    renderer_opts.push_constant_range = push_constant_range;
    renderer_opts.swap_chain.extent = game.m_swap_chain->get_extent();
    renderer_opts.swap_chain.render_pass = &game.m_world_render_pass;
    renderer_opts.quad.texture = graphics_pipeline::Texture::from_filepath(
        ctx, game.m_command_buffer_manager.get(), ASSET_FILE("sprite_sheet.png"));

    game.m_quad_renderer = std::make_unique<graphics_pipeline::quad::QuadRenderer>(
        ctx, game.m_command_buffer_manager.get(), renderer_opts);

    game.m_geom_renderer =
        std::make_unique<graphics_pipeline::geometry::GeometryRenderer>(
            ctx, game.m_command_buffer_manager.get(), renderer_opts);
}

void Configuration::setup_ui_renderers(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx, CaravanDefence &game) {

    DEBUG_ASSERT(game.m_swap_chain != nullptr,
                 "Error: Swap chain needs to be initialised before ui renderers.");

    DEBUG_ASSERT(
        game.m_command_buffer_manager != nullptr,
        "Error: Command buffer manager needs to be initialised before ui renderers.");

    DEBUG_ASSERT(game.m_ui_render_pass != nullptr,
                 "Error: UI render pass needs to be initialised before ui renderers.");

    vulkan::PushConstantRange push_constant_range;
    push_constant_range.offset = 0;
    push_constant_range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    push_constant_range.size = camera::Camera2D::matrix_size();

    graphics_pipeline::RendererOpts ui_renderer_opts{};
    ui_renderer_opts.push_constant_range = push_constant_range;
    ui_renderer_opts.swap_chain.extent = game.m_swap_chain->get_extent();
    ui_renderer_opts.swap_chain.render_pass = &game.m_ui_render_pass;

    game.m_ui_text_renderer =
        std::make_unique<graphics_pipeline::text::TextRenderer>(ctx, ui_renderer_opts);
    game.m_ui_text_renderer->load_font(
        game.m_command_buffer_manager.get(),
        font::FontLoader(ASSET_FILE("rabbid-highway-sign-iv-bold-oblique.otf")));

    game.m_ui_geom_renderer =
        std::make_unique<graphics_pipeline::geometry::GeometryRenderer>(
            ctx, game.m_command_buffer_manager.get(), ui_renderer_opts);
}

void Configuration::setup_mouse_event_handler(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx, CaravanDefence &game) {
    ctx->window->register_mouse_event_callback([&game](window::MouseEvent mouse_event,
                                                       interface::ViewportPoint &point)
                                                   -> void {
        switch (mouse_event) {
        case window::MouseEvent::RIGHT_BUTTON_DOWN:
            game.m_mouse_state.is_right_button_pressed = true;
            break;
        case window::MouseEvent::RIGHT_BUTTON_UP: {
            game.m_mouse_state.is_right_button_pressed = false;
            break;
        }
        case window::MouseEvent::CURSOR_MOVED:
            if (game.m_game_state.mode == GameMode::Playing &&
                game.m_mouse_state.is_right_button_pressed) {
                camera::WorldPoint2D world_delta = game.m_camera.viewport_delta_to_world(
                    point - game.m_mouse_state.cursor_viewport_position);
                game.m_camera.set_relative_position(world_delta * INVERT_AXISES);
            }
            game.m_mouse_state.cursor_viewport_position = point;
            break;
        case window::MouseEvent::SCROLL:
            if (game.m_game_state.mode == GameMode::Playing) {
                game.m_camera.set_relative_zoom(point.y() * ZOOM_SCALE_FACTOR);
            }
            break;
        case window::MouseEvent::LEFT_BUTTON_DOWN:
            break;
        case window::MouseEvent::LEFT_BUTTON_UP: {
            if (game.m_game_state.mode == GameMode::Playing) {
                const camera::WorldPoint2D world_point =
                    game.m_camera.viewport_to_world(point);
                const auto &current_selected_guard = game.m_game_state.selected_guard;

                // Clear currently selected guard
                if (current_selected_guard.has_value()) {
                    game.m_game_state.guards[current_selected_guard.value()].set_selected(
                        false);
                    game.m_game_state.guards[current_selected_guard.value()]
                        .set_highlighted(false);
                }

                const auto new_selected_guard = game.find_selected_guard(world_point);
                const auto clicked_caravan_slot =
                    game.find_selected_caravan_slot(world_point);

                if (current_selected_guard.has_value() &&
                    clicked_caravan_slot.has_value()) {
                    entity::Entity &slot =
                        game.m_game_state.caravan_slots[clicked_caravan_slot.value()];
                    entity::Entity &guard =
                        game.m_game_state.guards[current_selected_guard.value()];

                    if (entity::is_free(slot)) {
                        entity::Entity *old_slot = entity::get_caravan_slot(guard);
                        entity::set_caravan_slot(guard, &slot);
                        entity::set_occupying_guard(slot, &guard);

                        entity::clear_occupying_guard(*old_slot);
                        old_slot->set_highlighted(false);
                    }
                }

                if (new_selected_guard.has_value()) {
                    game.m_game_state.guards[new_selected_guard.value()].set_selected(
                        true);
                    game.m_game_state.guards[new_selected_guard.value()].set_highlighted(
                        true);
                }

                game.m_game_state.selected_guard = new_selected_guard;

            } else if (game.m_game_state.mode == GameMode::Event &&
                       game.m_game_state.event.has_value()) {
                const interface::NDCPoint cursor_ndc_point = game.m_camera.to_ndc_point(
                    game.m_mouse_state.cursor_viewport_position);
                game.m_game_state.event->on_click(cursor_ndc_point);
            }
            break;
        }
        }
    });
}

void Configuration::setup_keyboard_event_handler(
    std::shared_ptr<vulkan::context::GraphicsContext> &ctx, CaravanDefence &game) {

    ctx->window->register_keyboard_event_callback(
        [&game](window::KeyEvent &key, window::KeyState &state) -> void {
            if (state != window::KeyState::DOWN) {
                return;
            }

            switch (key) {
            case window::KeyEvent::Y: {
                if (game.m_game_state.mode == GameMode::Playing) {
                    game.m_game_state.event = game.create_event();
                    game.m_game_state.mode = GameMode::Event;
                }
                break;
            }

            default:
                break;
            };
        });
}

void Configuration::setup_initial_game_state(
    graphics_pipeline::geometry::GeometryRenderer *geom_renderer,
    graphics_pipeline::quad::QuadRenderer *quad_renderer, GameState &game_state) {

    DEBUG_ASSERT(
        geom_renderer != nullptr,
        "Error: Geometry renderer needs to be initialised before setting up game state.");

    DEBUG_ASSERT(
        quad_renderer != nullptr,
        "Error: Quad renderer needs to be initialised before setting up game state.");

    const float slot_distance_x = 250.0f;
    // Add entities
    game_state.caravan.push_back(
        entity::Entity::create_caravan_cart(camera::WorldPoint2D(0.0f, 0.0f)));
    game_state.caravan.back().set_render_data(quad_renderer, geom_renderer);
    game_state.caravan.back().set_uvwt(0.3f, 0.0f, 0.4f, 0.2f);
    game_state.caravan_slots.push_back(
        entity::Entity::create_caravan_slot(camera::WorldPoint2D(
            slot_distance_x, game_state.caravan.back().get_world_position().y)));
    game_state.caravan_slots.back().set_render_data(quad_renderer, geom_renderer);
    game_state.caravan_slots.push_back(
        entity::Entity::create_caravan_slot(camera::WorldPoint2D(
            -slot_distance_x, game_state.caravan.back().get_world_position().y)));
    game_state.caravan_slots.back().set_render_data(quad_renderer, geom_renderer);

    game_state.caravan.push_back(
        entity::Entity::create_caravan_cart(camera::WorldPoint2D(0.0f, 275.0f)));
    game_state.caravan.back().set_render_data(quad_renderer, geom_renderer);
    game_state.caravan.back().set_uvwt(0.2f, 0.0f, 0.3f, 0.2f);
    game_state.caravan_slots.push_back(
        entity::Entity::create_caravan_slot(camera::WorldPoint2D(
            slot_distance_x, game_state.caravan.back().get_world_position().y)));
    game_state.caravan_slots.back().set_render_data(quad_renderer, geom_renderer);
    game_state.caravan_slots.push_back(
        entity::Entity::create_caravan_slot(camera::WorldPoint2D(
            -slot_distance_x, game_state.caravan.back().get_world_position().y)));
    game_state.caravan_slots.back().set_render_data(quad_renderer, geom_renderer);

    game_state.guards.reserve(16); // 16 is magic
    game_state.guards.push_back(
        entity::Entity::create_guard(camera::WorldPoint2D(0.0f, 0.0f)));
    game_state.guards.push_back(
        entity::Entity::create_guard(camera::WorldPoint2D(0.0f, 0.0f)));

    set_damage_type(game_state.guards[0], entity::DamageType::A);
    set_damage_type(game_state.guards[1], entity::DamageType::B);

    game_state.enemies.reserve(64);
    game_state.attacks.reserve(8);

    for (size_t i = 0; i < game_state.guards.size(); i++) {
        game_state.guards[i].set_render_data(quad_renderer, geom_renderer);
        entity::set_caravan_slot(game_state.guards[i], &game_state.caravan_slots[i]);
        entity::set_occupying_guard(game_state.caravan_slots[i], &game_state.guards[i]);
    }
}
