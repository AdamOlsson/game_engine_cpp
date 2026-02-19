#pragma once
#include "camera/Camera.h"
#include "graphics_pipeline/geometry/GeometryRenderer.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "math/Matrix.h"
#include "math/Vector2.h"
#include "math/Vector4.h"
#include "math/interpolate.h"
#include "math/shape.h"
#include <variant>
namespace entity {

class Entity;

struct caravan_cart_t {
    util::colors::Color color = util::colors::rgb(0.6f, 0.0f, 0.6f);
    static constexpr util::colors::Color highlighted_color =
        util::colors::rgb(0.6f, 0.0f, 0.6f);
    static constexpr util::colors::Color selected_color =
        util::colors::rgb(0.6f, 0.0f, 0.6f);
    static constexpr math::Vector2 size = math::Vector2(128.0f, 256.0f);
    static constexpr float velocity = 0.0f;
    static constexpr float max_health = 10.0f;

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

    caravan_slot_t() {}
    ~caravan_slot_t() {}
};

enum class EnemyType : uint32_t { A = 0, B = 1 };

struct enemy_t {
    util::colors::Color color = util::colors::DARK_RED;
    static constexpr util::colors::Color highlighted_color = util::colors::DARK_RED;
    static constexpr util::colors::Color selected_color = util::colors::DARK_RED;
    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);
    static constexpr float velocity = 75.0f;
    static constexpr float max_health = 3.0f;

    static constexpr size_t spawn_rate_ms = 5000;

    static constexpr util::colors::Color type_a_color = util::colors::DARK_RED;
    static constexpr util::colors::Color type_b_color = util::colors::DARK_ORANGE;
    EnemyType type = EnemyType::A;

    enemy_t() {}
    ~enemy_t() {}
};

enum class DamageType : uint32_t { A = 0, B = 1 };

struct guard_t {
    util::colors::Color color = util::colors::rgb(0.0f, 0.55f, 0.0f);
    static constexpr util::colors::Color highlighted_color =
        util::colors::rgb(0.0f, 0.75f, 0.0f);
    static constexpr util::colors::Color selected_color =
        util::colors::rgb(0.0f, 0.75f, 0.0f);
    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);
    static constexpr float velocity = 95.0f;
    static constexpr float max_health = 10.0f;

    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double>;

    Entity *caravan_slot = nullptr;
    TimePoint last_attack = Clock::now();
    static constexpr float attack_range = 500.0f;
    static constexpr Duration attack_cooldown = std::chrono::seconds(3);

    DamageType damage_type = DamageType::A;

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

    ranged_attack_t() {}
    ~ranged_attack_t() {}
};

template <typename T>
concept has_color_field = requires(T t) {
    { t.color } -> std::convertible_to<util::colors::Color>;
};

template <typename T>
concept has_highlighted_color_field = requires(T t) {
    { t.highlighted_color } -> std::convertible_to<util::colors::Color>;
};

template <typename T>
concept has_selected_color_field = requires(T t) {
    { t.selected_color } -> std::convertible_to<util::colors::Color>;
};

template <typename T>
concept has_size_field = requires(T t) {
    { t.size } -> std::convertible_to<math::Vector2>;
};

template <typename T>
concept has_velocity_field = requires(T t) {
    { t.velocity } -> std::convertible_to<float>;
};

template <typename... Ts> constexpr bool all_have_color_field(std::variant<Ts...> *) {
    return (has_color_field<Ts> && ...);
}

template <typename... Ts>
constexpr bool all_have_highlighted_color_field(std::variant<Ts...> *) {
    return (has_highlighted_color_field<Ts> && ...);
}

template <typename... Ts>
constexpr bool all_have_selected_color_field(std::variant<Ts...> *) {
    return (has_selected_color_field<Ts> && ...);
}

template <typename... Ts> constexpr bool all_have_size_field(std::variant<Ts...> *) {
    return (has_color_field<Ts> && ...);
}

template <typename... Ts> constexpr bool all_have_velocity_field(std::variant<Ts...> *) {
    return (has_velocity_field<Ts> && ...);
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

    math::Matrix m_model_matrix;
    bool m_is_highlighted = false;
    bool m_is_selected = false;
    bool m_is_visible = true;
    EntityState m_current_action = EntityState::Idle;

    util::colors::Color m_color;
    math::Vector2 m_size;
    float m_rotation_rad = 0;

    graphics_pipeline::quad::QuadRenderer *m_quad_renderer = nullptr;
    std::optional<graphics_pipeline::quad::QuadSBOHandle> m_render_data_handle =
        std::nullopt;

    graphics_pipeline::geometry::GeometryRenderer *m_geometry_renderer = nullptr;
    std::optional<graphics_pipeline::geometry::GeometrySBOHandle>
        m_highlight_render_data = std::nullopt;

    struct {
        float max = 100.0f;
        float current = 100.0f;
        float width = 10.0f;
        float damage_multiplier = 1.0f;
        math::Vector2 offset;
        std::optional<graphics_pipeline::geometry::GeometrySBOHandle> bar = std::nullopt;

        template <typename T> void init() {
            max = T::max_health;
            current = T::max_health;
        }
    } m_health;

    struct {
        float velocity = 0.0f;
        camera::WorldPoint2D target = math::Vector2(0.0f, 0.0f);
    } m_movement;

    static_assert(all_have_color_field(static_cast<EntityVariant *>(nullptr)),
                  "All entity variants must have a color field.");
    static_assert(all_have_highlighted_color_field(static_cast<EntityVariant *>(nullptr)),
                  "All entity variants must have a highlighted_color field.");
    static_assert(all_have_selected_color_field(static_cast<EntityVariant *>(nullptr)),
                  "All entity variants must have a selected_color field.");
    static_assert(all_have_size_field(static_cast<EntityVariant *>(nullptr)),
                  "All entity variants must have a size field.");
    static_assert(all_have_velocity_field(static_cast<EntityVariant *>(nullptr)),
                  "All entity variants must have a velocity field.");

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
          m_model_matrix(std::move(model)), m_color(get_color()), m_size(get_size()) {
        m_health.init<T>();
        m_movement.velocity = T::velocity;
        m_movement.target = get_world_position();
    }

  public:
    Entity() = default;

    Entity(Entity &&other) noexcept = default;
    Entity &operator=(Entity &&other) noexcept = default;

    // Delete copy constructor and operator
    Entity(const Entity &other) = delete;
    Entity &operator=(const Entity &other) = delete;

    template <typename T> bool holds() const { return std::holds_alternative<T>(m_type); }
    template <typename T> T &get() { return std::get<T>(m_type); }

    static Entity create_caravan_cart(const camera::WorldPoint2D &position) {
        math::Matrix model =
            math::Matrix().translate(position).scale(caravan_cart_t::size);
        return Entity(std::in_place_type<caravan_cart_t>, std::move(model));
    }

    static Entity create_caravan_slot(const camera::WorldPoint2D &position) {
        math::Matrix model =
            math::Matrix().translate(position).scale(caravan_slot_t::size);
        return Entity(std::in_place_type<caravan_slot_t>, std::move(model));
    }

    static Entity create_guard(const camera::WorldPoint2D &position) {
        math::Matrix model = math::Matrix().translate(position).scale(guard_t::size);
        return Entity(std::in_place_type<guard_t>, std::move(model));
    }

    static Entity create_enemy(const camera::WorldPoint2D &position) {
        math::Matrix model = math::Matrix().translate(position).scale(enemy_t::size);
        return Entity(std::in_place_type<enemy_t>, std::move(model));
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
        return e;
    }

    void set_render_data(graphics_pipeline::quad::QuadRenderer *quad_renderer,
                         graphics_pipeline::geometry::GeometryRenderer *geom_renderer) {
        m_quad_renderer = quad_renderer;
        m_render_data_handle = m_quad_renderer->request_render_slot();
        auto &instance = m_quad_renderer->get_instance(m_render_data_handle.value());
        instance.model_matrix = m_model_matrix;

        if (holds<enemy_t>()) {
            instance.texture_id = 0;
            instance.sampling_mode =
                static_cast<uint32_t>(graphics_pipeline::quad::TextureSamplerMode::SDF);
            instance.uvwt = math::Vector4(0.0f, 0.0f, 0.2f, 0.2f);

            enemy_t &enemy = get<enemy_t>();
            switch (enemy.type) {
            case EnemyType::A:
                m_color = enemy_t::type_a_color;
                break;
            case EnemyType::B:
                m_color = enemy_t::type_b_color;
                break;
            }
        } else if (holds<guard_t>()) {
            guard_t &guard = get<guard_t>();
            switch (guard.damage_type) {
            case DamageType::A:
                m_color = enemy_t::type_a_color;
                break;
            case DamageType::B:
                m_color = enemy_t::type_b_color;
                break;
            }
        }
        instance.color = m_color;

        m_geometry_renderer = geom_renderer;

        const auto entity_size = m_size;
        m_health.offset =
            std::move(math::Vector2(0.0f, (entity_size.y() / 2.0f) + 20.0f));
        m_health.width = entity_size.x();
        m_health.bar = m_geometry_renderer->request_render_slot();
        auto &health_bar = m_geometry_renderer->get_instance(m_health.bar.value());
        health_bar.flags |=
            static_cast<uint32_t>(graphics_pipeline::geometry::GeometryShape::Rectangle);
        health_bar.model_matrix = math::Matrix()
                                      .translate(get_world_position() + m_health.offset)
                                      .scale(m_health.width, 10.0f);

        if (holds<enemy_t>() || holds<caravan_cart_t>()) {
            health_bar.color = util::colors::rgb(0.0f, 8.0f, 0.0f);
        } else {
            health_bar.color = util::colors::TRANSPARENT;
        }
    }

    void clear_render_data() {
        if (m_quad_renderer != nullptr) {
            m_quad_renderer->return_render_slot(m_render_data_handle.value());
            m_quad_renderer = nullptr;
        }

        if (m_geometry_renderer != nullptr) {
            if (m_health.bar.has_value()) {
                m_geometry_renderer->return_render_slot(m_health.bar.value());
            }
            m_geometry_renderer = nullptr;
        }
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
        if (m_quad_renderer != nullptr) {
            auto &instance = m_quad_renderer->get_instance(m_render_data_handle.value());
            instance.model_matrix = m_model_matrix;
        }
        update_health();
    }

    camera::WorldPoint2D get_world_position() const {
        return m_model_matrix.position_2d();
    }

    bool is_visible() const { return m_is_visible; }
    void toggle_visibility() { set_visibility(!m_is_visible); }
    void set_visibility(const bool is_visible) {
        m_is_visible = is_visible;
        if (m_quad_renderer != nullptr) {
            auto &instance = m_quad_renderer->get_instance(m_render_data_handle.value());
            instance.color = m_is_visible ? m_color : util::colors::TRANSPARENT;
        }
    }

    bool is_highlighted() { return m_is_highlighted; }
    void toggle_highlighted() { set_highlighted(!m_is_highlighted); }
    void set_highlighted(bool is_highlighted) {
        const bool old_value = m_is_highlighted;
        m_is_highlighted = is_highlighted;
        handle_highlight(old_value, is_highlighted);
    }
    void handle_highlight(const bool prev_value, const bool new_value) {
        DEBUG_ASSERT(m_quad_renderer != nullptr,
                     "Error: Changing the highlight value of an entity requires the "
                     "render data to be set.");
        DEBUG_ASSERT(m_geometry_renderer != nullptr,
                     "Error: Changing the highlight value of an entity requires the "
                     "render data to be set.");
        if (m_quad_renderer == nullptr || prev_value == new_value) {
            return;
        }

        auto &instance = m_quad_renderer->get_instance(m_render_data_handle.value());
        instance.color = new_value ? get_highlighted_color() : m_color;

        if (holds<guard_t>()) {
            if (prev_value) {
                DEBUG_ASSERT(
                    m_highlight_render_data.has_value(),
                    "Error: Expected highlight render data if guard is highlighted.");
                m_geometry_renderer->return_render_slot(m_highlight_render_data.value());
                m_highlight_render_data = std::nullopt;
            } else {
                DEBUG_ASSERT(
                    !m_highlight_render_data.has_value(),
                    "Error: Did not expect highlight render data if guard is not "
                    "highlighted.");
                const guard_t &guard = get<guard_t>();
                m_highlight_render_data = m_geometry_renderer->request_render_slot();
                auto &highlight_instance =
                    m_geometry_renderer->get_instance(m_highlight_render_data.value());
                highlight_instance.flags |= static_cast<uint32_t>(
                    graphics_pipeline::geometry::GeometryShape::Circle);
                highlight_instance.color = util::colors::rgba(0.8f, 0.8f, 0.8f, 0.01f);
                highlight_instance.model_matrix =
                    math::Matrix()
                        .translate(get_world_position())
                        .scale(guard.attack_range * 2, guard.attack_range * 2);
            }
        }
    }

    bool is_selected() { return m_is_selected; }
    void toggle_selected() { set_selected(!m_is_selected); }
    void set_selected(const bool is_selected) {
        m_is_selected = is_selected;
        if (m_quad_renderer != nullptr) {
            auto &instance = m_quad_renderer->get_instance(m_render_data_handle.value());
            instance.color = m_is_selected ? get_selected_color() : m_color;
        }
    }

    float get_current_health() { return m_health.current; }
    float get_max_health() { return m_health.max; }
    void update_health() {
        if (m_geometry_renderer != nullptr && m_health.bar.has_value()) {
            auto &health_bar = m_geometry_renderer->get_instance(m_health.bar.value());

            health_bar.model_matrix =
                math::Matrix()
                    .translate(get_world_position() + m_health.offset)
                    .scale(m_health.width * (m_health.current / m_health.max), 10.0f);
        }
    }

    void damage(const float dmg) { m_health.current -= dmg; }
    void kill() { m_health.current = 0.0f; }
    bool is_dead() { return m_health.current <= 0.0f; }
    bool is_alive() { return m_health.current > 0.0f; }

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
        }
    }

    void set_move_target(const camera::WorldPoint2D &target) {
        m_movement.target = target;
    }

    void set_uvwt(const float u, const float v, const float w, const float t) {
        auto &instance = m_quad_renderer->get_instance(m_render_data_handle.value());
        instance.uvwt = math::Vector4(u, v, w, t);
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

inline bool in_attack_range(Entity &guard_entity, const Entity &enemy_entity) {
    DEBUG_ASSERT(guard_entity.holds<guard_t>(), "Error: Expected guard.");
    auto &guard = guard_entity.get<guard_t>();
    const camera::WorldPoint2D &guard_position = guard_entity.get_world_position();
    const float attack_range = guard.attack_range;
    return math::distance2(guard_position, enemy_entity.get_world_position()) <
           (attack_range * attack_range);
}

inline bool can_attack(Entity &guard_entity) {
    DEBUG_ASSERT(guard_entity.holds<guard_t>(), "Error: Expected guard.");
    auto &guard = guard_entity.get<guard_t>();
    return (guard_t::Clock::now() - guard.last_attack) > guard.attack_cooldown &&
           guard_entity.is_attacking();
}

inline EnemyType get_enemy_type(Entity &enemy_entity);
inline Entity attack(Entity &guard_entity, Entity &enemy_entity) {
    DEBUG_ASSERT(guard_entity.holds<guard_t>(), "Error: Expected guard.");
    auto &guard = guard_entity.get<guard_t>();
    guard.last_attack = guard_t::Clock::now();

    const uint enemy_type = static_cast<uint>(get_enemy_type(enemy_entity));
    const uint guard_type = static_cast<uint>(guard.damage_type);
    float damage = 1.0f;
    if (enemy_type == guard_type) {
        damage = 3.0f;
    }
    enemy_entity.damage(damage);

    return Entity::create_ranged_attack(guard_entity.get_world_position(),
                                        enemy_entity.get_world_position());
}

inline void set_damage_type(Entity &guard_entity, const DamageType type) {
    DEBUG_ASSERT(guard_entity.holds<guard_t>(), "Error: Expected guard.");
    auto &guard = guard_entity.get<guard_t>();
    guard.damage_type = type;
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

inline void set_enemy_type(Entity &enemy_entity, const EnemyType type) {
    DEBUG_ASSERT(enemy_entity.holds<enemy_t>(), "Error: Expected enemy.");
    auto &enemy = enemy_entity.get<enemy_t>();
    enemy.type = type;
}

inline EnemyType get_enemy_type(Entity &enemy_entity) {
    DEBUG_ASSERT(enemy_entity.holds<enemy_t>(), "Error: Expected enemy.");
    auto &enemy = enemy_entity.get<enemy_t>();
    return enemy.type;
}

} // namespace entity
