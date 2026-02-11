#pragma once
#include "camera/Camera.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "math/Matrix.h"
#include "math/Vector2.h"
#include "math/shape.h"
#include <variant>
namespace entity {

class Entity;

struct caravan_t {
    static constexpr util::colors::Color color = util::colors::MAGENTA;
    static constexpr math::Vector2 size = math::Vector2(100.0f, 200.0f);
};

struct caravan_slot_t {
    static constexpr util::colors::Color color =
        util::colors::rgba(0.5f, 0.5f, 0.5f, 0.2f);
    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);

    static constexpr util::colors::Color highlighted_color =
        util::colors::rgba(0.5f, 0.5f, 0.5f, 0.8f);
    Entity *occupying_guard = nullptr;
};

struct enemy_t {
    static constexpr util::colors::Color color = util::colors::RED;
    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);

    ~enemy_t() {
        throw std::runtime_error("Error: enemy_t desctuctor not yet implemented.");
    }
};

struct guard_t {
    static constexpr util::colors::Color color = util::colors::GREEN;
    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);

    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using Duration = std::chrono::duration<double>;

    static constexpr util::colors::Color m_selected_color = util::colors::YELLOW;
    Entity *caravan_slot = nullptr;
    TimePoint m_last_attack = Clock::now();
    static constexpr float m_attack_range = 300.0f;
    static constexpr Duration m_attack_cooldown = std::chrono::seconds(3);
};

struct ranged_attack_t {
    static constexpr util::colors::Color color = util::colors::ORANGE;
    static constexpr math::Vector2 size = math::Vector2(50.0f, 100.0f);
};

template <typename T>
concept has_color_field = requires(T t) {
    { t.color } -> std::convertible_to<util::colors::Color>;
};

template <typename T>
concept has_size_field = requires(T t) {
    { t.size } -> std::convertible_to<math::Vector2>;
};

template <typename... Ts> constexpr bool all_have_color_field(std::variant<Ts...> *) {
    return (has_color_field<Ts> && ...);
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

    std::optional<graphics_pipeline::quad::QuadSBOHandle> m_render_data_handle;

    static_assert(all_have_color_field(static_cast<EntityVariant *>(nullptr)),
                  "All entity variants must have a color field.");
    static_assert(all_have_size_field(static_cast<EntityVariant *>(nullptr)),
                  "All entity variants must have a size field.");

    constexpr util::colors::Color get_color() const {
        return std::visit([](const auto &entity) { return entity.color; }, m_type);
    }

    constexpr math::Vector2 get_size() const {
        return std::visit([](const auto &entity) { return entity.size; }, m_type);
    }

  public:
    Entity() = default;

    template <typename T, typename... Args>
        requires(std::is_same_v<T, caravan_t> || std::is_same_v<T, caravan_slot_t> ||
                 std::is_same_v<T, enemy_t> || std::is_same_v<T, guard_t>)
    Entity(std::in_place_type_t<T>, const camera::WorldPoint2D &position)
        : m_type(std::in_place_type<T>),
          m_model_matrix(math::Matrix().translate(position).scale(get_size())) {}

    Entity(Entity &&other) noexcept = default;
    Entity &operator=(Entity &&other) noexcept = default;

    // Delete copy constructor and operator
    Entity(const Entity &other) = delete;
    Entity &operator=(const Entity &other) = delete;

    template <typename... Args> static Entity create_caravan(Args &&...args) {
        return Entity(std::in_place_type<caravan_t>, std::forward<Args>(args)...);
    }

    template <typename... Args> static Entity create_caravan_slot(Args &&...args) {
        return Entity(std::in_place_type<caravan_slot_t>, std::forward<Args>(args)...);
    }

    template <typename... Args> static Entity create_guard(Args &&...args) {
        return Entity(std::in_place_type<guard_t>, std::forward<Args>(args)...);
    }

    void set_render_data(graphics_pipeline::quad::QuadSBOHandle &&render_data_handle) {
        m_render_data_handle = std::move(render_data_handle);
        m_render_data_handle->data->model_matrix = m_model_matrix;
        m_render_data_handle->data->color = get_color();
    }

    bool is_point_inside(const camera::WorldPoint2D &point) {
        // World position with no regard to the world grid
        const camera::WorldPoint2D position = m_model_matrix.position_2d();
        const math::Vector2 size = get_size();
        return math::is_point_inside_rectangle(point, position, size.x, size.y);
    }

    void set_world_position(const camera::WorldPoint2D &position) {
        m_model_matrix = math::Matrix().translate(position).scale(get_size());
        if (m_render_data_handle.has_value()) {
            m_render_data_handle->data->model_matrix = m_model_matrix;
        }
    }
    camera::WorldPoint2D get_world_position() const {
        return m_model_matrix.position_2d();
    }

    bool is_visible() const { return m_is_visible; }
    void toggle_visibility() { set_visibility(!m_is_visible); }

    void set_visibility(const bool is_visible) {
        m_is_visible = is_visible;
        if (m_render_data_handle.has_value()) {
            m_render_data_handle->data->color =
                m_is_visible ? get_color() : util::colors::TRANSPARENT;
        }
    }

    /*#################################################*/
    /*################## CARAVAN SLOT #################*/
    /*#################################################*/

    template <typename T>
        requires std::is_same_v<T, caravan_slot_t>
    void toggle_highlighted() {
        set_highlighted<T>(!m_is_highlighted);
    }

    template <typename T>
        requires std::is_same_v<T, caravan_slot_t>
    void set_highlighted(bool is_highlighted) {
        m_is_highlighted = is_highlighted;
        if (m_render_data_handle.has_value()) {
            m_render_data_handle->data->color =
                m_is_highlighted ? std::get<T>(m_type).highlighted_color : get_color();
        }
    }

    template <typename T, typename G>
        requires std::is_same_v<T, caravan_slot_t> && std::is_same_v<G, guard_t>
    void set_occupying_guard(G *guard) {
        DEBUG_ASSERT(guard != nullptr,
                     "Error: Setting guard slot with nullptr, use "
                     "clear_occupying_guard() to clear the occupying guard.");

        set_visibility(false);
        std::get<T>(m_type).set_highlighted(false);
        std::get<T>(m_type).occupying_guard = guard;
    }

    template <typename T>
        requires std::is_same_v<T, caravan_slot_t>
    void clear_occupying_guard() {
        set_visibility(true);
        std::get<T>(m_type).set_highlighted(false);
        std::get<T>(m_type).occupying_guard = nullptr;
    }

    template <typename T>
        requires std::is_same_v<T, caravan_slot_t>
    Entity *get_occupying_guard() {
        return std::get<T>(m_type).occupying_guard;
    }

    template <typename T>
        requires std::is_same_v<T, caravan_slot_t>
    bool is_occupied() const {
        return std::get<T>(m_type).occupying_guard != nullptr;
    }

    template <typename T>
        requires std::is_same_v<T, caravan_slot_t>
    bool is_free() const {
        return std::get<T>(m_type).occupying_guard == nullptr;
    }

    /*#################################################*/
    /*##################### GUARD #####################*/
    /*#################################################*/

    template <typename T>
        requires std::is_same_v<T, guard_t>
    bool set_selected(const bool is_selected) {
        m_is_selected = is_selected;
        if (m_render_data_handle.has_value()) {
            m_render_data_handle->data->color =
                m_is_selected ? std::get<T>(m_type).m_selected_color : get_color();
        }
    }

    template <typename T>
        requires std::is_same_v<T, guard_t>
    bool toggle_selected() {
        std::get<T>(m_type).set_selected(!m_is_selected);
    }

    template <typename T, typename S>
        requires std::is_same_v<T, guard_t> && std::is_same_v<S, caravan_slot_t>
    void set_caravan_slot(S *slot) {
        DEBUG_ASSERT(slot != nullptr, "Error: setting caravan slot with a nullptr, use "
                                      "clear_caraval_slot() to clear the slot.");
        std::get<T>(m_type).caravan_slot = slot;
        set_world_position(slot->get_world_position());
    }

    template <typename T>
        requires std::is_same_v<T, guard_t>
    void clear_caravan_slot() {
        std::get<T>(m_type).caravan_slot = nullptr;
    }

    template <typename T>
        requires std::is_same_v<T, guard_t>
    Entity *get_caravan_slot() {
        return std::get<T>(m_type).caravan_slot;
    }

    template <typename T>
        requires std::is_same_v<T, guard_t>
    bool in_attack_range(const camera::WorldPoint2D &point) const {
        throw std::runtime_error("Error: in_attack_range() not yet implemented.");
    }

    template <typename T>
        requires std::is_same_v<T, guard_t>
    bool can_attack() const {
        throw std::runtime_error("Error: can_attack() not yet implemented.");
    }

    template <typename T, typename E>
        requires std::is_same_v<T, guard_t> && std::is_same_v<E, enemy_t>
    Entity attack() const {
        throw std::runtime_error("Error: attack() not yet implemented.");
    }

    /*#################################################*/
    /*##################### ENEMY #####################*/
    /*#################################################*/

    template <typename T>
        requires std::is_same_v<T, enemy_t>
    void move_towards(const camera::WorldPoint2D &target, const float dt) {
        throw std::runtime_error("Error: move_towards() not yet implemented.");
    }

    template <typename T>
        requires std::is_same_v<T, enemy_t>
    void die() {
        throw std::runtime_error("Error: die() not yet implemented.");
    }

    /*#################################################*/
    /*################ RANGED ATTACK ##################*/
    /*#################################################*/

    template <typename T>
        requires std::is_same_v<T, ranged_attack_t>
    void remove() {
        throw std::runtime_error("Error: remove() not yet implemented.");
    }

    template <typename T>
        requires std::is_same_v<T, ranged_attack_t>
    void update(const float dt_s) {
        throw std::runtime_error("Error: update() not yet implemented.");
    }
};

} // namespace entity
