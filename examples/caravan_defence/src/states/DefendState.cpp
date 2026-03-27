#include "DefendState.h"

#include "../GameState.h"
#include <vector>

constexpr void DefendState::spawn_group_of_enemies(GameState &game_state) {
    const size_t num_enemies = game_state.rng.uniform(1, 3);
    const float group_distance = game_state.rng.uniform(800.0f, 1200.0f);
    const float group_angle = game_state.rng.uniform(-45.0f, 225.0f);
    const math::Vector2 group_center =
        math::Vector2(group_distance, 0.0f).rotate_z(group_angle);

    const int group_radius = 200;
    for (size_t i = 0; i < num_enemies; i++) {
        const float distance_from_group_center =
            game_state.rng.uniform(-group_radius, group_radius);
        const float angle_from_group_center = game_state.rng.uniform(0, 360);

        const math::Vector2 enemy_position =
            math::Vector2(distance_from_group_center, 0.0f)
                .rotate_z(angle_from_group_center);

        const uint32_t enemy_type = game_state.rng.uniform(0, 1);

        game_state.enemies.push_back(
            entity::Entity::create_enemy(group_center + enemy_position));

        const size_t caravan_cart_id = game_state.rng.uniform(0, 1);
        game_state.enemies.back().set_move_target(
            game_state.caravan[caravan_cart_id].get_world_position());

        if (game_state.rng.uniform(0.0f, 1.0f) < 0.3f) {
            const size_t health_bar_type_id = game_state.rng.uniform(0, 1);
            HealthBarOpts health_bar_opts{};
            health_bar_opts.max_health = 3.0f;
            health_bar_opts.type = health_bar_type_id == 0 ? HealthBarType::EnergyShield
                                                           : HealthBarType::Armor;
            game_state.enemies.back().add_health_bar(health_bar_opts);
        }
    }
}

std::optional<size_t> DefendState::find_caravan_cart(GameState &game_state,
                                                     const camera::WorldPoint2D &point) {
    for (size_t i = 0; i < game_state.caravan.size(); i++) {
        if (game_state.caravan[i].is_point_inside(point)) {
            return i;
        }
    }
    return std::nullopt;
}

void DefendState::on_enter(GameState &game_state) {
    DEBUG_ASSERT(m_world_quad_renderer != nullptr,
                 "Error: Quad renderer is not set in DefendState.");
    DEBUG_ASSERT(m_world_geometry_renderer != nullptr,
                 "Error: Geometry renderer is not set in DefendState.");
};

void DefendState::on_exit(GameState &game_state) {
    DEBUG_ASSERT(m_world_quad_renderer != nullptr,
                 "Error: Quad renderer is not set in DefendState.");
    DEBUG_ASSERT(m_world_geometry_renderer != nullptr,
                 "Error: Geometry renderer is not set in DefendState.");
};

void DefendState::handle_cursor(const interface::NDCPoint &cursor_position,
                                const bool has_clicked) {}

util::StateTransition DefendState::update(const float dt, GameState &game_state) {
    DEBUG_ASSERT(m_world_quad_renderer != nullptr,
                 "Error: Quad renderer is not set in DefendState.");
    DEBUG_ASSERT(m_world_geometry_renderer != nullptr,
                 "Error: Geometry renderer is not set in DefendState.");

    const interface::NDCPoint ndc_point =
        game_state.camera.to_ndc_point(game_state.cursor.viewport_position);

    if (m_settings_panel.is_point_inside(ndc_point)) {
        m_settings_panel.handle_cursor(game_state);
    } else if (game_state.cursor.click_point.has_value()) {
        handle_click(game_state, game_state.cursor.click_point.value());
    }
    game_state.cursor.click_point = std::nullopt;

    handle_hover(game_state, game_state.cursor.viewport_position);

    update_all(dt, game_state.caravan);
    update_all(dt, game_state.caravan_slots);
    update_all(dt, game_state.attacks);
    update_all(dt, game_state.enemies);
    update_all(dt, game_state.guards);

    for (int i = game_state.attacks.size() - 1; i >= 0; i--) {
        if (!game_state.attacks[i].is_visible()) {
            game_state.attacks.erase(game_state.attacks.begin() + i);
        }
    }

    const size_t time_elapsed_ms = game_state.time_elapsed_ms;
    if (time_elapsed_ms > entity::enemy_t::spawn_rate_ms) {
        spawn_group_of_enemies(game_state);
        game_state.time_elapsed_ms = 0;
    }

    for (size_t i = 0; i < game_state.enemies.size(); i++) {
        entity::Entity &enemy = game_state.enemies[i];

        for (entity::Entity &guard : game_state.guards) {
            if (enemy.is_alive() && guard.can_attack() && guard.in_attack_range(enemy)) {

                // Create attack
                game_state.attacks.push_back(guard.attack(enemy));
                break;
            }
        }

        // Find the id of the cart the enemy is inside.
        auto cart_id = find_caravan_cart(game_state, enemy.get_world_position());
        if (enemy.is_alive() && cart_id.has_value()) {
            enemy.attack(game_state.caravan[cart_id.value()]);
            enemy.kill();
        }

        if (enemy.is_dead()) {
            game_state.enemies.erase(game_state.enemies.begin() + i);
        }
    }

    for (auto &cart : game_state.caravan) {
        if (cart.is_dead()) {
            logger::info("Cart died, game over!");
            exit(0);
        }
    }

    game_state.time_elapsed_ms += dt * 1000;

    return util::StateTransition::none();
}

EntitySettingsPanel DefendState::init_entity_settings_panel() {

    const float bbox_center_line_x = -0.7f;

    EntitySettingsPanel panel;
    panel.center_x = bbox_center_line_x;
    panel.is_open = false;
    panel.size = math::Vector2(0.6f, 2.0f);

    panel.bbox_render_data.color = EntitySettingsPanel::background_color;
    panel.bbox_render_data.border.color = EntitySettingsPanel::border_color;
    panel.bbox_render_data.border.width = 0.015f;
    panel.bbox_render_data.border.radius = 0.05f;
    panel.bbox_render_data.model_matrix =
        math::Matrix().translate(panel.center_x, 0.0f).scale(panel.size);

    auto text_opts = graphics_pipeline::text::TextOpts{};
    text_opts.font_color = EntitySettingsPanel::font_color;
    text_opts.font_size = 0.05f;
    text_opts.line_width = 0.20f;
    text_opts.position =
        math::Vector2(panel.center_x - text_opts.line_width / 2.0f, -0.90f);

    auto headline_handle = m_ui_text_renderer->create_text2("Guard", text_opts);
    panel.text_handles.push_back(std::move(headline_handle));

    DropDown drop_down{};
    drop_down.bbox.color = DropDown::background_color;
    drop_down.bbox.border.color = EntitySettingsPanel::border_color;
    drop_down.bbox.border.width = 0.005;
    drop_down.bbox.model_matrix =
        math::Matrix().translate(panel.center_x, -0.8f).scale(0.4f, 0.05f);

    const auto change_weapon_to_sniper = [](GameState &game_state) {
        DEBUG_ASSERT(
            game_state.selected_guard.has_value(),
            "Error: Settings panel is open for a guard, but no guard is selected.");
        game_state.guards[game_state.selected_guard.value()].set_weapon(
            Weapon::create_weapon<Sniper>());
    };

    const auto change_weapon_to_bow = [](GameState &game_state) {
        DEBUG_ASSERT(
            game_state.selected_guard.has_value(),
            "Error: Settings panel is open for a guard, but no guard is selected.");
        game_state.guards[game_state.selected_guard.value()].set_weapon(
            Weapon::create_weapon<Bow>());
    };

    const auto change_weapon_to_sword = [](GameState &game_state) {
        DEBUG_ASSERT(
            game_state.selected_guard.has_value(),
            "Error: Settings panel is open for a guard, but no guard is selected.");
        game_state.guards[game_state.selected_guard.value()].set_weapon(
            Weapon::create_weapon<Sword>());
    };

    drop_down.add_drop_down_item(change_weapon_to_sniper);
    drop_down.add_drop_down_item(change_weapon_to_bow);
    drop_down.add_drop_down_item(change_weapon_to_sword);

    panel.drop_downs.push_back(drop_down);

    return panel;
}

void DefendState::handle_click(GameState &game_state,
                               interface::ViewportPoint &click_position) {

    const camera::WorldPoint2D world_point =
        game_state.camera.viewport_to_world(click_position);

    const interface::NDCPoint ndc_point = game_state.camera.to_ndc_point(click_position);
    const auto &current_selected_guard = game_state.selected_guard;

    // Clear currently selected guard
    if (current_selected_guard.has_value()) {
        game_state.guards[current_selected_guard.value()].set_selected(false);
        game_state.guards[current_selected_guard.value()].set_highlighted(false);
        m_settings_panel.close();
    }

    const auto new_selected_guard = find_selected_guard(game_state, world_point);
    const auto clicked_caravan_slot = find_selected_caravan_slot(game_state, world_point);

    if (current_selected_guard.has_value() && clicked_caravan_slot.has_value()) {
        entity::Entity &slot = game_state.caravan_slots[clicked_caravan_slot.value()];
        entity::Entity &guard = game_state.guards[current_selected_guard.value()];

        if (entity::is_free(slot)) {
            entity::Entity *old_slot = entity::get_caravan_slot(guard);
            entity::set_caravan_slot(guard, &slot);
            entity::set_occupying_guard(slot, &guard);

            entity::clear_occupying_guard(*old_slot);
            old_slot->set_highlighted(false);
        }
    }

    if (new_selected_guard.has_value()) {
        game_state.guards[new_selected_guard.value()].set_selected(true);
        game_state.guards[new_selected_guard.value()].set_highlighted(true);
        m_settings_panel.open();
    }

    game_state.selected_guard = new_selected_guard;
}

void DefendState::handle_hover(GameState &game_state,
                               interface::ViewportPoint &cursor_position) {

    const camera::WorldPoint2D cursor_world_point =
        game_state.camera.viewport_to_world(cursor_position);

    // Only highlight slots when hover if a guard is selected
    if (game_state.selected_guard.has_value()) {
        for (size_t i = 0; i < game_state.caravan_slots.size(); i++) {
            if (!game_state.caravan_slots[i].is_visible()) {
                continue;
            }
            game_state.caravan_slots[i].set_highlighted(
                game_state.caravan_slots[i].is_point_inside(cursor_world_point));
        }
    }

    const auto &selected_guard = game_state.selected_guard;
    // Highlight the guard the cursor is hovering over
    for (size_t i = 0; i < game_state.guards.size(); i++) {
        entity::Entity &guard = game_state.guards[i];
        // If a guard is selected, we keep it highlighted
        if (selected_guard.has_value() && selected_guard.value() == i) {
            continue;
        }
        guard.set_highlighted(guard.is_point_inside(cursor_world_point));
    }
}

std::optional<size_t>
DefendState::find_selected_caravan_slot(GameState &game_state,
                                        const camera::WorldPoint2D &click_point) {
    for (size_t i = 0; i < game_state.caravan_slots.size(); i++) {
        if (game_state.caravan_slots[i].is_point_inside(click_point)) {
            return i;
        }
    }
    return std::nullopt;
}

std::optional<size_t>
DefendState::find_selected_guard(GameState &game_state,
                                 const camera::WorldPoint2D &click_point) {
    for (size_t i = 0; i < game_state.guards.size(); i++) {
        if (game_state.guards[i].is_point_inside(click_point)) {
            return i;
        }
    }
    return std::nullopt;
}
