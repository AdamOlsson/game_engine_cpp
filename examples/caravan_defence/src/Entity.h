#pragma once
#include "camera/Camera.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "math/Matrix.h"
#include "math/Vector2.h"
#include "math/interpolate.h"
#include "math/shape.h"
#include <variant>
namespace entity {

class Entity;

struct caravan_t {
    static constexpr util::colors::Color color = util::colors::MAGENTA;
    static constexpr util::colors::Color highlighted_color = util::colors::MAGENTA;
    static constexpr util::colors::Color selected_color = util::colors::MAGENTA;
    static constexpr math::Vector2 size = math::Vector2(100.0f, 200.0f);

    caravan_t() {}
    ~caravan_t() {}
};

struct caravan_slot_t {
    static constexpr util::colors::Color color =
        util::colors::rgba(0.5f, 0.5f, 0.5f, 0.2f);
    static constexpr util::colors::Color highlighted_color =
        util::colors::rgba(0.5f, 0.5f, 0.5f, 0.8f);
    static constexpr util::colors::Color selected_color =
        util::colors::rgba(0.5f, 0.5f, 0.5f, 0.2f);
    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);

    Entity *occupying_guard = nullptr;

    caravan_slot_t() {}
    ~caravan_slot_t() {}
};

struct enemy_t {
    static constexpr util::colors::Color color = util::colors::RED;
    static constexpr util::colors::Color highlighted_color = util::colors::RED;
    static constexpr util::colors::Color selected_color = util::colors::RED;
    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);

    static constexpr float velocity = 25.0f;
    static constexpr size_t spawn_rate_ms = 3000;

    enemy_t() {}
    ~enemy_t() {}
};

struct guard_t {
    static constexpr util::colors::Color color = util::colors::GREEN;
    static constexpr util::colors::Color highlighted_color = util::colors::GREEN;
    static constexpr util::colors::Color selected_color = util::colors::YELLOW;

    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);

    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double>;

    Entity *caravan_slot = nullptr;
    TimePoint last_attack = Clock::now();
    static constexpr float attack_range = 300.0f;
    static constexpr Duration attack_cooldown = std::chrono::seconds(3);

    guard_t() {}
    ~guard_t() {}
};

struct ranged_attack_t {
    static constexpr util::colors::Color color = util::colors::ORANGE;
    static constexpr util::colors::Color highlighted_color = util::colors::ORANGE;
    static constexpr util::colors::Color selected_color = util::colors::ORANGE;
    static constexpr math::Vector2 size = math::Vector2(0.0f, 10.0f);

    float lifetime_count = 0.0f;
    static constexpr float lifetime_ms = 1000.0f;

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

class Entity {
  private:
    using EntityVariant =
        std::variant<caravan_t, caravan_slot_t, enemy_t, guard_t, ranged_attack_t>;
    EntityVariant m_type;

    math::Matrix m_model_matrix;
    bool m_is_highlighted = false;
    bool m_is_selected = false;
    bool m_is_visible = true;

    graphics_pipeline::quad::QuadSBOHandle m_render_data_handle;
    graphics_pipeline::quad::QuadRenderer *m_quad_renderer = nullptr;

    static_assert(all_have_color_field(static_cast<EntityVariant *>(nullptr)),
                  "All entity variants must have a color field.");
    static_assert(all_have_highlighted_color_field(static_cast<EntityVariant *>(nullptr)),
                  "All entity variants must have a highlighted_color field.");
    static_assert(all_have_selected_color_field(static_cast<EntityVariant *>(nullptr)),
                  "All entity variants must have a selected_color field.");
    static_assert(all_have_size_field(static_cast<EntityVariant *>(nullptr)),
                  "All entity variants must have a size field.");

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

    template <typename T, typename... Args>
        requires(std::is_same_v<T, caravan_t> || std::is_same_v<T, caravan_slot_t> ||
                 std::is_same_v<T, enemy_t> || std::is_same_v<T, guard_t> ||
                 std::is_same_v<T, ranged_attack_t>)
    Entity(std::in_place_type_t<T>, math::Matrix &&model, Args &&...args)
        : m_type(std::in_place_type<T>, std::forward<Args>(args)...),
          m_model_matrix(std::move(model)) {}

  public:
    Entity() = default;

    Entity(Entity &&other) noexcept = default;
    Entity &operator=(Entity &&other) noexcept = default;

    // Delete copy constructor and operator
    Entity(const Entity &other) = delete;
    Entity &operator=(const Entity &other) = delete;

    template <typename T> bool holds() const { return std::holds_alternative<T>(m_type); }
    template <typename T> T &get() { return std::get<T>(m_type); }

    static Entity create_caravan(const camera::WorldPoint2D &position) {
        math::Matrix model = math::Matrix().translate(position).scale(caravan_t::size);
        return Entity(std::in_place_type<caravan_t>, std::move(model));
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
        const float height = ranged_attack_t::size.y;
        const camera::WorldPoint2D local_vec = end - start;
        const float rotation = math::angle_to_x_axis(local_vec);
        math::Matrix model = math::Matrix()
                                 .translate(start + local_vec / 2.0f)
                                 .rotate_z(rotation)
                                 .scale(width, height, 0.0f);
        return Entity(std::in_place_type<ranged_attack_t>, std::move(model));
    }

    void set_render_data(graphics_pipeline::quad::QuadRenderer *renderer) {
        m_quad_renderer = renderer;
        m_render_data_handle = m_quad_renderer->request_render_slot();
        auto &instance = m_quad_renderer->get_instance(m_render_data_handle);
        instance.model_matrix = m_model_matrix;
        instance.color = get_color();
    }

    void clear_render_data() {
        if (m_quad_renderer != nullptr) {
            m_quad_renderer->return_render_slot(m_render_data_handle);
            m_quad_renderer = nullptr;
        }
    }

    bool is_point_inside(const camera::WorldPoint2D &point) {
        // World position with no regard to the world grid
        const camera::WorldPoint2D position = m_model_matrix.position_2d();
        const math::Vector2 size = get_size();
        return math::is_point_inside_rectangle(point, position, size.x, size.y);
    }

    void set_world_position(const camera::WorldPoint2D &position) {
        m_model_matrix = math::Matrix().translate(position).scale(get_size());
        if (m_quad_renderer != nullptr) {
            auto &instance = m_quad_renderer->get_instance(m_render_data_handle);
            instance.model_matrix = m_model_matrix;
        }
    }

    camera::WorldPoint2D get_world_position() const {
        return m_model_matrix.position_2d();
    }

    bool is_visible() const { return m_is_visible; }
    void toggle_visibility() { set_visibility(!m_is_visible); }
    void set_visibility(const bool is_visible) {
        m_is_visible = is_visible;
        if (m_quad_renderer != nullptr) {
            auto &instance = m_quad_renderer->get_instance(m_render_data_handle);
            instance.color = m_is_visible ? get_color() : util::colors::TRANSPARENT;
        }
    }

    bool is_highlighted() { return m_is_highlighted; }
    void toggle_highlighted() { set_highlighted(!m_is_highlighted); }
    void set_highlighted(bool is_highlighted) {
        m_is_highlighted = is_highlighted;
        if (m_quad_renderer != nullptr) {
            auto &instance = m_quad_renderer->get_instance(m_render_data_handle);
            instance.color = m_is_highlighted ? get_highlighted_color() : get_color();
        }
    }

    bool is_selected() { return m_is_selected; }
    void toggle_selected() { set_selected(!m_is_selected); }
    void set_selected(const bool is_selected) {
        m_is_selected = is_selected;
        if (m_quad_renderer != nullptr) {
            auto &instance = m_quad_renderer->get_instance(m_render_data_handle);
            instance.color = m_is_selected ? get_selected_color() : get_color();
        }
    }

    void update(const float dt_s) {
        std::visit(
            [this, dt_s](const auto &entity) {
                using T = std::decay_t<decltype(entity)>;
                if constexpr (std::is_same_v<T, ranged_attack_t>) {
                    ranged_attack_t &attack = std::get<ranged_attack_t>(m_type);

                    if (attack.lifetime_count > attack.lifetime_ms) {
                        m_is_visible = false;
                    }

                    attack.lifetime_count += dt_s * 1000.0f;

                } else {
                }
            },
            m_type);
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
    slot_entity.set_highlighted(false);
}

inline Entity *get_occupying_guard(Entity &slot_entity) {
    DEBUG_ASSERT(slot_entity.holds<caravan_slot_t>(), "Error: Expected caravan slot.");
    auto &slot = slot_entity.template get<caravan_slot_t>();
    return slot.occupying_guard;
}

inline bool is_occupied(CaravanSlotEntity auto &slot_entity) {
    auto &slot = slot_entity.template get<caravan_slot_t>();
    return slot.occupying_guard != nullptr;
}

inline bool is_free(Entity &slot_entity) {
    DEBUG_ASSERT(slot_entity.holds<caravan_slot_t>(), "Error: Expected caravan slot.");
    auto &slot = slot_entity.get<caravan_slot_t>();
    return slot.occupying_guard == nullptr;
}

inline void set_caravan_slot(Entity &guard_entity, Entity *slot_entity) {
    DEBUG_ASSERT(guard_entity.holds<guard_t>(), "Error: Expected guard.");
    DEBUG_ASSERT(slot_entity != nullptr,
                 "Error: setting caravan slot with a nullptr, use "
                 "clear_caraval_slot() to clear the slot.");
    auto &guard = guard_entity.get<guard_t>();
    guard.caravan_slot = slot_entity;
    guard_entity.set_world_position(slot_entity->get_world_position());
}

inline void clear_caravan_slot(Entity &guard_entity) {
    DEBUG_ASSERT(guard_entity.holds<guard_t>(), "Error: Expected guard.");
    auto &guard = guard_entity.get<guard_t>();
    guard.caravan_slot = nullptr;
}

inline Entity *get_caravan_slot(Entity &guard_entity) {
    DEBUG_ASSERT(guard_entity.holds<guard_t>(), "Error: Expected guard.");
    auto &guard = guard_entity.template get<guard_t>();
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
    return (guard_t::Clock::now() - guard.last_attack) > guard.attack_cooldown;
}

inline Entity attack(Entity &guard_entity, const Entity &enemy_entity) {
    DEBUG_ASSERT(guard_entity.holds<guard_t>(), "Error: Expected guard.");
    auto &guard = guard_entity.get<guard_t>();
    guard.last_attack = guard_t::Clock::now();
    return Entity::create_ranged_attack(guard_entity.get_world_position(),
                                        enemy_entity.get_world_position());
}

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
