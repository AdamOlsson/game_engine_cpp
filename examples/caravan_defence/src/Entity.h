#pragma once
#include "Health.h"
#include "Weapon.h"
#include "camera/Camera.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "math/Matrix.h"
#include "math/Vector2.h"
#include "math/Vector4.h"
#include "math/interpolate.h"
#include "math/shape.h"
#include <variant>
namespace entity {

enum class PreferredTarget {
    Nearest,
    Farthest,
    Armor,
    EnergyShield,
};

class Entity;

using Clock = std::chrono::steady_clock;

struct caravan_cart_t {
    util::colors::Color color = util::colors::rgb(0.6f, 0.0f, 0.6f);
    static constexpr util::colors::Color highlighted_color =
        util::colors::rgb(0.6f, 0.0f, 0.6f);
    static constexpr util::colors::Color selected_color =
        util::colors::rgb(0.6f, 0.0f, 0.6f);
    static constexpr math::Vector2 size = math::Vector2(128.0f, 256.0f);
    static constexpr float velocity = 0.0f;
    static constexpr float max_health = 10.0f;
    static constexpr std::optional<Weapon> weapon = std::nullopt;

    caravan_cart_t() {}
    ~caravan_cart_t() {}
};

struct caravan_slot_t {
    util::colors::Color color = util::colors::rgba(0.5f, 0.5f, 0.5f, 0.2f);
    static constexpr util::colors::Color highlighted_color =
        util::colors::rgba(0.5f, 0.5f, 0.5f, 0.8f);
    static constexpr util::colors::Color selected_color =
        util::colors::rgba(0.5f, 0.5f, 0.5f, 0.2f);
    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);
    static constexpr float velocity = 0.0f;
    static constexpr float max_health = 10.0f;

    Entity *occupying_guard = nullptr;
    static constexpr std::optional<Weapon> weapon = std::nullopt;

    caravan_slot_t() {}
    ~caravan_slot_t() {}
};

struct enemy_t {
    static constexpr util::colors::Color color = util::colors::DARK_RED;
    static constexpr util::colors::Color highlighted_color = util::colors::DARK_RED;
    static constexpr util::colors::Color selected_color = util::colors::DARK_RED;
    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);
    static constexpr float velocity = 75.0f;
    static constexpr float max_health = 3.0f;

    static constexpr size_t spawn_rate_ms = 5000;

    static constexpr std::optional<Weapon> weapon = Weapon::create_weapon<Sword>();

    enemy_t() {}
    ~enemy_t() {}
};

struct guard_t {
    util::colors::Color color = util::colors::hex(0xFFEA00);
    static constexpr util::colors::Color highlighted_color =
        util::colors::rgb(0.0f, 0.75f, 0.0f);
    static constexpr util::colors::Color selected_color =
        util::colors::rgb(0.0f, 0.75f, 0.0f);
    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);
    static constexpr float velocity = 95.0f;
    static constexpr float max_health = 10.0f;

    Entity *caravan_slot = nullptr;

    static constexpr std::optional<Weapon> weapon = Weapon::create_weapon<Bow>();

    guard_t() {}
    ~guard_t() {}
};

struct ranged_attack_t {
    util::colors::Color color = util::colors::ORANGE;
    static constexpr util::colors::Color highlighted_color = util::colors::ORANGE;
    static constexpr util::colors::Color selected_color = util::colors::ORANGE;
    static constexpr math::Vector2 size = math::Vector2(0.0f, 10.0f);
    static constexpr float velocity = 0.0f;
    static constexpr float max_health = 10.0f;

    float lifetime_count = 0.0f;
    static constexpr float lifetime_ms = 800.0f;

    static constexpr std::optional<Weapon> weapon = std::nullopt;

    ranged_attack_t() {}
    ~ranged_attack_t() {}
};

class AttackRangeRenderData {
  private:
    float m_range = 1.0f;

  public:
    graphics_pipeline::geometry::GeometryPipelineSBO data;

    AttackRangeRenderData() = default;
    AttackRangeRenderData(const math::Vector2 &position, const float range)
        : m_range(range) {
        data.flags |=
            static_cast<uint32_t>(graphics_pipeline::geometry::GeometryShape::Circle);
        data.color = util::colors::rgba(0.8f, 0.8f, 0.8f, 0.01f);
        data.model_matrix =
            math::Matrix().translate(position).scale(m_range * 2, m_range * 2);
    }

    void set_position(const math::Vector2 &position) {
        data.model_matrix =
            math::Matrix().translate(position).scale(m_range * 2, m_range * 2);
    }
};

template <typename T>
concept is_valid_entity = requires(T t) {
    { t.velocity } -> std::convertible_to<float>;
    { t.size } -> std::convertible_to<math::Vector2>;
    { t.selected_color } -> std::convertible_to<util::colors::Color>;
    { t.highlighted_color } -> std::convertible_to<util::colors::Color>;
    { t.color } -> std::convertible_to<util::colors::Color>;
    { t.weapon } -> std::convertible_to<std::optional<Weapon>>;
};

template <typename... Ts> constexpr bool validate_entities(std::variant<Ts...> *) {
    return (is_valid_entity<Ts> && ...);
}

enum EntityState {
    Idle,
    Moving,
    Attacking,
};

class Entity {
  private:
    using EntityVariant =
        std::variant<caravan_cart_t, caravan_slot_t, enemy_t, guard_t, ranged_attack_t>;
    EntityVariant m_type;

    static_assert(validate_entities(static_cast<EntityVariant *>(nullptr)),
                  "All entity variantes are not valid.");

    math::Matrix m_model_matrix;
    bool m_is_highlighted = false;
    bool m_is_selected = false;
    bool m_is_visible = true;
    EntityState m_current_action = EntityState::Idle;

    util::colors::Color m_color;
    math::Vector2 m_size;
    float m_rotation_rad = 0;

    graphics_pipeline::quad::QuadPipelineSBO m_render_data;

    PreferredTarget m_preferred_target = PreferredTarget::Farthest;
    std::optional<Health> m_health = std::nullopt;
    std::optional<Weapon> m_weapon = std::nullopt;
    std::optional<AttackRangeRenderData> m_attack_range_render_data = std::nullopt;

    struct {
        float velocity = 0.0f;
        camera::WorldPoint2D target = math::Vector2(0.0f, 0.0f);
    } m_movement;

    constexpr util::colors::Color get_color() const {
        return std::visit([](const auto &entity) { return entity.color; }, m_type);
    }

    constexpr util::colors::Color get_highlighted_color() const {
        return std::visit([](const auto &entity) { return entity.highlighted_color; },
                          m_type);
    }

    constexpr util::colors::Color get_selected_color() const {
        return std::visit([](const auto &entity) { return entity.selected_color; },
                          m_type);
    }

    constexpr math::Vector2 get_size() const {
        return std::visit([](const auto &entity) { return entity.size; }, m_type);
    }

    constexpr float get_max_health() const {
        return std::visit([](const auto &entity) { return entity.max_health; }, m_type);
    }

    template <typename T, typename... Args>
        requires(std::is_same_v<T, caravan_cart_t> || std::is_same_v<T, caravan_slot_t> ||
                 std::is_same_v<T, enemy_t> || std::is_same_v<T, guard_t> ||
                 std::is_same_v<T, ranged_attack_t>)
    Entity(std::in_place_type_t<T>, math::Matrix &&model, Args &&...args)
        : m_type(std::in_place_type<T>, std::forward<Args>(args)...),
          m_model_matrix(std::move(model)), m_color(get_color()), m_size(get_size()),
          m_weapon(T::weapon) {
        m_movement.velocity = T::velocity;
        m_movement.target = get_world_position();
    }

  public:
    Entity() = default;

    Entity(Entity &&other) noexcept = default;
    Entity &operator=(Entity &&other) noexcept = default;

    Entity(const Entity &other) = delete;
    Entity &operator=(const Entity &other) = delete;

    template <typename T> bool holds() const { return std::holds_alternative<T>(m_type); }
    template <typename T> T &get() { return std::get<T>(m_type); }

    static Entity create_caravan_cart(const camera::WorldPoint2D &position) {
        math::Matrix model =
            math::Matrix().translate(position).scale(caravan_cart_t::size);
        auto e = Entity(std::in_place_type<caravan_cart_t>, std::move(model));
        math::Vector2 health_bar_position_offset =
            math::Vector2(0.0f, (e.m_size.y() / 2.0f) + 20.0f);
        math::Vector2 health_bar_position =
            e.get_world_position() + health_bar_position_offset;
        math::Vector2 health_bar_size = math::Vector2(e.m_size.x(), 10.0f);

        HealthBarOpts health_bar_opts{};
        health_bar_opts.type = HealthBarType::Normal;
        health_bar_opts.max_health = e.get_max_health();

        e.m_health = Health(health_bar_size, health_bar_opts);
        e.m_health->set_health_bar_offset(health_bar_position_offset);
        e.m_health->set_health_bar_position(health_bar_position);
        e.m_render_data.color = e.m_color;
        return e;
    }

    static Entity create_caravan_slot(const camera::WorldPoint2D &position) {
        math::Matrix model =
            math::Matrix().translate(position).scale(caravan_slot_t::size);
        auto e = Entity(std::in_place_type<caravan_slot_t>, std::move(model));
        e.m_render_data.color = e.m_color;
        return e;
    }

    static Entity create_guard(const camera::WorldPoint2D &position) {
        math::Matrix model = math::Matrix().translate(position).scale(guard_t::size);
        auto e = Entity(std::in_place_type<guard_t>, std::move(model));
        const float attack_range = e.m_weapon.has_value() ? e.m_weapon->range : 0.0f;
        e.m_attack_range_render_data =
            AttackRangeRenderData(e.get_world_position(), attack_range);
        e.m_render_data.color = e.m_color;
        return e;
    }

    static Entity create_enemy(const camera::WorldPoint2D &position) {
        math::Matrix model = math::Matrix().translate(position).scale(enemy_t::size);
        auto e = Entity(std::in_place_type<enemy_t>, std::move(model));

        e.m_render_data.texture_id = 0;
        e.m_render_data.sampling_mode =
            static_cast<uint32_t>(graphics_pipeline::quad::TextureSamplerMode::SDF);
        e.m_render_data.uvwt = math::Vector4(0.0f, 0.0f, 0.2f, 0.2f);
        e.m_render_data.color = enemy_t::color;
        e.m_color = enemy_t::color;

        math::Vector2 health_bar_position_offset =
            math::Vector2(0.0f, (e.m_size.y() / 2.0f) + 20.0f);
        math::Vector2 health_bar_position =
            e.get_world_position() + health_bar_position_offset;
        math::Vector2 health_bar_size = math::Vector2(e.m_size.x(), 10.0f);

        HealthBarOpts health_bar_opts{};
        health_bar_opts.type = HealthBarType::Normal;
        health_bar_opts.max_health = e.get_max_health();

        e.m_health = Health(health_bar_size, health_bar_opts);
        e.m_health->set_health_bar_offset(health_bar_position_offset);
        e.m_health->set_health_bar_position(health_bar_position);

        return e;
    }

    static Entity create_ranged_attack(const camera::WorldPoint2D &start,
                                       const camera::WorldPoint2D &end) {
        const float width = math::distance(start, end);
        const float height = ranged_attack_t{}.size.y();
        const camera::WorldPoint2D local_vec = end - start;
        const float rotation = math::angle_to_x_axis(local_vec);
        math::Matrix model = math::Matrix()
                                 .translate(start + local_vec / 2.0f)
                                 .rotate_z(rotation)
                                 .scale(width, height, 0.0f);
        auto e = Entity(std::in_place_type<ranged_attack_t>, std::move(model));
        e.m_size.x() = width;
        e.m_size.y() = height;
        e.m_rotation_rad = rotation;
        e.set_move_target(model.position_2d());

        e.m_render_data.color = e.m_color;

        return e;
    }

    void get_geometry_render_data(
        std::vector<graphics_pipeline::geometry::GeometryPipelineSBO> &out) const {

        if (is_highlighted() && m_attack_range_render_data.has_value()) {
            out.push_back(m_attack_range_render_data->data);
        }

        if (m_health.has_value()) {
            const auto &health = m_health->get_render_data();
            for (const auto &h : health) {
                out.push_back(h);
            }
        }
    }

    void get_quad_render_data(
        std::vector<graphics_pipeline::quad::QuadPipelineSBO> &out) const {

        out.push_back(m_render_data);
    }

    bool is_point_inside(const camera::WorldPoint2D &point) {
        // World position with no regard to the world grid
        const camera::WorldPoint2D position = m_model_matrix.position_2d();
        const math::Vector2 size = m_size;
        return math::is_point_inside_rectangle(point, position, size.x(), size.y());
    }

    void set_world_position(const camera::WorldPoint2D &position) {
        m_model_matrix =
            math::Matrix().translate(position).rotate_z(m_rotation_rad).scale(m_size);
        m_render_data.model_matrix = m_model_matrix;
        update_health();
    }

    camera::WorldPoint2D get_world_position() const {
        return m_model_matrix.position_2d();
    }

    bool is_visible() const { return m_is_visible; }
    void toggle_visibility() { set_visibility(!m_is_visible); }
    void set_visibility(const bool is_visible) { m_is_visible = is_visible; }

    bool is_highlighted() const { return m_is_highlighted; }
    void toggle_highlighted() { set_highlighted(!m_is_highlighted); }
    void set_highlighted(bool is_highlighted) {
        const bool old_value = m_is_highlighted;
        m_is_highlighted = is_highlighted;
        handle_highlight(old_value, is_highlighted);
    }
    void handle_highlight(const bool prev_value, const bool new_value) {
        m_render_data.color = new_value ? get_highlighted_color() : m_color;
    }

    bool is_selected() { return m_is_selected; }
    void toggle_selected() { set_selected(!m_is_selected); }
    void set_selected(const bool is_selected) {
        m_is_selected = is_selected;
        m_render_data.color = m_is_selected ? get_selected_color() : m_color;
    }

    float get_current_health() {
        DEBUG_ASSERT(m_health.has_value(), "Error: Trying to call get_current_health() "
                                           "on an entity that has no health.");
        return m_health->current_health();
    }

    void update_health() {
        if (m_health.has_value()) {
            m_health->set_health_bar_position(get_world_position());
            m_health->update_health_bar();
        }
    }

    HealthBarType get_active_health_bar_type() const {
        return m_health->get_active_health_bar_type();
    }

    void damage(const Weapon &weapon) {
        DEBUG_ASSERT(m_health.has_value(), "Error: Trying to call damage() "
                                           "on an entity that has no health.");
        const HealthBarType current_hp_type = m_health->get_active_health_bar_type();

        float damage = weapon.damage;

        if (current_hp_type == HealthBarType::Normal &&
            weapon.damage_type != DamageType::Physical) {
            damage = weapon.damage / 2.0f;
        } else if (current_hp_type == HealthBarType::EnergyShield &&
                   weapon.damage_type != DamageType::Energy) {
            damage = weapon.damage / 2.0f;
        } else if (current_hp_type == HealthBarType::Armor &&
                   weapon.damage_type != DamageType::Physical) {
            damage = weapon.damage / 2.0f;
        }

        m_health->handle_incomming_damage(damage);
    }

    void kill() {
        DEBUG_ASSERT(m_health.has_value(), "Error: Trying to call kill() "
                                           "on an entity that has no health.");
        m_health->handle_incomming_damage(m_health->max_health());
    }

    bool is_dead() const { return m_health->is_dead(); }
    bool is_alive() const { return !m_health->is_dead(); }

    void set_color(const util::colors::Color &color) { m_color = color; }

    EntityState get_state() { return m_current_action; }
    bool is_attacking() { return m_current_action == EntityState::Attacking; }
    bool is_moving() { return m_current_action == EntityState::Moving; }
    bool is_idle() { return m_current_action == EntityState::Idle; }

    void update(const float dt_s) {

        const camera::WorldPoint2D position = get_world_position();
        const float distance = math::distance(position, m_movement.target);

        camera::WorldPoint2D new_position = position;
        if (distance > 1.0f) {
            const float movement = m_movement.velocity * dt_s;
            const float fraction = std::min(movement / distance, 1.0f);
            new_position = math::lerp(position, m_movement.target, fraction);
            m_current_action = EntityState::Moving;
        } else {
            m_current_action = EntityState::Attacking;
        }

        set_world_position(new_position);
        update_health();

        if (holds<ranged_attack_t>()) {
            ranged_attack_t &attack = get<ranged_attack_t>();
            if (attack.lifetime_count > attack.lifetime_ms) {
                m_is_visible = false;
            }
            attack.lifetime_count += dt_s * 1000.0f;
        } else if (holds<caravan_slot_t>()) {
            caravan_slot_t &slot = get<caravan_slot_t>();
            if (slot.occupying_guard != nullptr) {
                set_visibility(slot.occupying_guard->is_moving());
            }
        } else if (holds<guard_t>()) {
            m_attack_range_render_data->set_position(new_position);
        }
    }

    void set_move_target(const camera::WorldPoint2D &target) {
        m_movement.target = target;
    }

    void set_uvwt(const float u, const float v, const float w, const float t) {
        m_render_data.uvwt = math::Vector4(u, v, w, t);
    }

    void add_health_bar(const HealthBarOpts &opts) {
        if (m_health.has_value()) {
            m_health->add_health_bar(opts);
        }
    }

    void set_preferred_target(const PreferredTarget target) {
        m_preferred_target = target;
    }

    void set_weapon(Weapon &&weapon) {
        m_weapon = std::move(weapon);
        m_attack_range_render_data =
            AttackRangeRenderData(get_world_position(), m_weapon->range);
    }

    size_t select_target(const std::vector<Entity> &enemies,
                         const std::vector<Entity> &caravan_carts) {
        DEBUG_ASSERT(
            m_weapon.has_value(),
            "Error: Checking select_target() but entity does not have a weapon.");

        size_t nearest_enemy = std::numeric_limits<size_t>::max();
        float nearest_enemy_distance = std::numeric_limits<float>::max();

        size_t farthest_enemy = std::numeric_limits<size_t>::max();
        float farthest_enemy_distance = 0.0f;

        size_t target_candidate = std::numeric_limits<size_t>::max();
        float target_candidate_distance = std::numeric_limits<float>::max();
        for (size_t i = 0; i < enemies.size(); i++) {
            const auto &enemy = enemies[i];
            if (enemy.is_dead()) {
                continue;
            }

            if (!in_attack_range(enemy)) {
                continue;
            }

            // find the closest caravan cart
            float this_enemy_min_distance_to_cart = std::numeric_limits<float>::max();
            float this_enemy_max_distance_to_cart = 0.0f;
            for (size_t j = 0; j < caravan_carts.size(); j++) {
                const auto &cart = caravan_carts[j];
                const float distance2 = math::distance2(enemy.get_world_position(),
                                                        cart.get_world_position());
                this_enemy_min_distance_to_cart =
                    fmin(this_enemy_min_distance_to_cart, distance2);
                this_enemy_max_distance_to_cart =
                    fmax(this_enemy_max_distance_to_cart, distance2);
            }

            if (this_enemy_min_distance_to_cart < nearest_enemy_distance) {
                nearest_enemy = i;
                nearest_enemy_distance = this_enemy_min_distance_to_cart;
            }

            if (this_enemy_max_distance_to_cart > farthest_enemy_distance) {
                farthest_enemy = i;
                farthest_enemy_distance = this_enemy_max_distance_to_cart;
            }

            switch (m_preferred_target) {
            case PreferredTarget::Nearest: {
                target_candidate = nearest_enemy;
                target_candidate_distance = nearest_enemy_distance;
                break;
            }
            case PreferredTarget::Farthest: {
                target_candidate = farthest_enemy;
                target_candidate_distance = farthest_enemy_distance;
                break;
            }
            case PreferredTarget::Armor:
                if (enemy.get_active_health_bar_type() == HealthBarType::Armor &&
                    this_enemy_min_distance_to_cart < target_candidate_distance) {
                    target_candidate = i;
                    target_candidate_distance = this_enemy_min_distance_to_cart;
                }
                break;
            case PreferredTarget::EnergyShield:
                if (enemy.get_active_health_bar_type() == HealthBarType::EnergyShield &&
                    this_enemy_min_distance_to_cart < target_candidate_distance) {
                    target_candidate = i;
                    target_candidate_distance = this_enemy_min_distance_to_cart;
                }
                break;
            }
        }

        // If we found no enemy matching the preferred target, set target to the nearest
        if (target_candidate == std::numeric_limits<size_t>::max()) {
            target_candidate = nearest_enemy;
        }

        return target_candidate;
    }

    bool in_attack_range(const Entity &target) {
        DEBUG_ASSERT(
            m_weapon.has_value(),
            "Error: Checking in_attack_range() but entity does not have a weapon.");
        const camera::WorldPoint2D &position = get_world_position();
        const float attack_range = m_weapon->range;
        return math::distance2(position, target.get_world_position()) <
               (attack_range * attack_range);
    }

    bool can_attack() {
        DEBUG_ASSERT(m_weapon.has_value(),
                     "Error: Checking can_attack() but entity does not have a weapon.");
        return m_weapon->can_fire() && is_attacking();
    }

    Entity attack(Entity &target_entity) {
        DEBUG_ASSERT(m_weapon.has_value(),
                     "Error: Calling attack() but entity does not have a weapon.");
        m_weapon->fire();
        target_entity.damage(m_weapon.value());

        return Entity::create_ranged_attack(get_world_position(),
                                            target_entity.get_world_position());
    }
};

template <typename E>
concept CaravanSlotEntity = requires(E &e) {
    { e.template get<caravan_slot_t>() } -> std::same_as<caravan_slot_t &>;
};

template <typename E>
concept GuardEntity = requires(E &e) {
    { e.template get<guard_t>() } -> std::same_as<guard_t &>;
};

template <typename E>
concept EnemyEntity = requires(E &e) {
    { e.template get<enemy_t>() } -> std::same_as<enemy_t &>;
};

// ###################################################
// ################### Caravan Slot ##################
// ###################################################
inline void set_occupying_guard(Entity &slot_entity, Entity *guard_entity) {
    DEBUG_ASSERT(slot_entity.holds<caravan_slot_t>(), "Error: Expected caravan slot.");
    auto &slot = slot_entity.get<caravan_slot_t>();
    slot.occupying_guard = guard_entity;
    slot_entity.set_visibility(false);
}

inline void clear_occupying_guard(Entity &slot_entity) {
    DEBUG_ASSERT(slot_entity.holds<caravan_slot_t>(), "Error: Expected caravan slot.");
    auto &slot = slot_entity.get<caravan_slot_t>();
    slot.occupying_guard = nullptr;
    slot_entity.set_visibility(true);
}

inline Entity *get_occupying_guard(Entity &slot_entity) {
    DEBUG_ASSERT(slot_entity.holds<caravan_slot_t>(), "Error: Expected caravan slot.");
    auto &slot = slot_entity.get<caravan_slot_t>();
    return slot.occupying_guard;
}

inline bool is_occupied(Entity &slot_entity) {
    DEBUG_ASSERT(slot_entity.holds<caravan_slot_t>(), "Error: Expected caravan slot.");
    auto &slot = slot_entity.get<caravan_slot_t>();
    return slot.occupying_guard != nullptr;
}

inline bool is_free(Entity &slot_entity) {
    DEBUG_ASSERT(slot_entity.holds<caravan_slot_t>(), "Error: Expected caravan slot.");
    auto &slot = slot_entity.get<caravan_slot_t>();
    return slot.occupying_guard == nullptr;
}

// ###################################################
// ###################### Guard ######################
// ###################################################
inline void set_caravan_slot(Entity &guard_entity, Entity *slot_entity) {
    DEBUG_ASSERT(guard_entity.holds<guard_t>(), "Error: Expected guard.");
    DEBUG_ASSERT(slot_entity != nullptr,
                 "Error: setting caravan slot with a nullptr, use "
                 "clear_caraval_slot() to clear the slot.");
    auto &guard = guard_entity.get<guard_t>();
    guard.caravan_slot = slot_entity;
    /*guard_entity.set_world_position(slot_entity->get_world_position());*/
    guard_entity.set_move_target(slot_entity->get_world_position());
}

inline void clear_caravan_slot(Entity &guard_entity) {
    DEBUG_ASSERT(guard_entity.holds<guard_t>(), "Error: Expected guard.");
    auto &guard = guard_entity.get<guard_t>();
    guard.caravan_slot = nullptr;
}

inline Entity *get_caravan_slot(Entity &guard_entity) {
    DEBUG_ASSERT(guard_entity.holds<guard_t>(), "Error: Expected guard.");
    auto &guard = guard_entity.get<guard_t>();
    return guard.caravan_slot;
}

// ###################################################
// ###################### Enemy ######################
// ###################################################
inline void move_towards(Entity &enemy_entity, const camera::WorldPoint2D &target,
                         const float dt) {
    DEBUG_ASSERT(enemy_entity.holds<enemy_t>(), "Error: Expected enemy.");
    auto &enemy = enemy_entity.get<enemy_t>();

    const camera::WorldPoint2D position = enemy_entity.get_world_position();
    const float distance = math::distance(position, target);
    const float movement = enemy.velocity * dt;

    const float fraction = std::min(movement / distance, 1.0f);
    camera::WorldPoint2D new_position = math::lerp(position, target, fraction);

    enemy_entity.set_world_position(new_position);
}

} // namespace entity
