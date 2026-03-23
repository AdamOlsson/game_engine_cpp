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
        game_state.enemies.back().set_render_data(m_quad_renderer, m_geometry_renderer);

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
    DEBUG_ASSERT(m_quad_renderer != nullptr,
                 "Error: Quad renderer is not set in DefendState.");
    DEBUG_ASSERT(m_geometry_renderer != nullptr,
                 "Error: Geometry renderer is not set in DefendState.");
};

void DefendState::on_exit(GameState &game_state) {
    DEBUG_ASSERT(m_quad_renderer != nullptr,
                 "Error: Quad renderer is not set in DefendState.");
    DEBUG_ASSERT(m_geometry_renderer != nullptr,
                 "Error: Geometry renderer is not set in DefendState.");
};

util::StateTransition DefendState::update(const float dt, GameState &game_state) {
    DEBUG_ASSERT(m_quad_renderer != nullptr,
                 "Error: Quad renderer is not set in DefendState.");
    DEBUG_ASSERT(m_geometry_renderer != nullptr,
                 "Error: Geometry renderer is not set in DefendState.");

    if (game_state.cursor.click_point.has_value()) {
        handle_cursor(game_state, game_state.cursor.click_point.value());
        game_state.cursor.click_point = std::nullopt;
    }

    update_all(dt, game_state.caravan);
    update_all(dt, game_state.caravan_slots);
    update_all(dt, game_state.attacks);
    update_all(dt, game_state.enemies);
    update_all(dt, game_state.guards);

    for (int i = game_state.attacks.size() - 1; i >= 0; i--) {
        if (!game_state.attacks[i].is_visible()) {
            game_state.attacks[i].clear_render_data();
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
                game_state.attacks.back().set_render_data(m_quad_renderer,
                                                          m_geometry_renderer);
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
            enemy.clear_render_data();
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

    // Return nullopt to indicate no state transition (stay in current state)
    return util::StateTransition::none();
}

void DefendState::handle_cursor(GameState &game_state,
                                interface::ViewportPoint &click_point) {

    const camera::WorldPoint2D world_point =
        game_state.camera.viewport_to_world(click_point);
    const auto &current_selected_guard = game_state.selected_guard;

    // Clear currently selected guard
    if (current_selected_guard.has_value()) {
        game_state.guards[current_selected_guard.value()].set_selected(false);
        game_state.guards[current_selected_guard.value()].set_highlighted(false);
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
    }

    game_state.selected_guard = new_selected_guard;
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
