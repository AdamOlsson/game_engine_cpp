#pragma once

#include <chrono>
#include <variant>

enum class DamageType {
    Physical,
    Energy,
};

using Clock = std::chrono::steady_clock;
using Duration = std::chrono::duration<double>;
using TimePoint = std::chrono::time_point<Clock, Duration>;

struct Bow {
    static constexpr float damage = 2.0f;
    static constexpr DamageType damage_type = DamageType::Physical;
    static constexpr float range = 500.0f;
    static constexpr Duration fire_rate = std::chrono::seconds(3);
};

struct Sniper {
    static constexpr float damage = 10.0f;
    static constexpr DamageType damage_type = DamageType::Physical;
    static constexpr float range = 1500.0f;
    static constexpr Duration fire_rate = std::chrono::seconds(6);
};

struct Sword {
    static constexpr float damage = 3.0f;
    static constexpr DamageType damage_type = DamageType::Physical;
    static constexpr float range = 150.0f;
    static constexpr Duration fire_rate = std::chrono::milliseconds(500);
};

template <typename T>
concept is_valid_weapon = requires(T t) {
    { t.damage } -> std::convertible_to<float>;
    { t.damage_type } -> std::convertible_to<DamageType>;
    { t.range } -> std::convertible_to<float>;
    { t.fire_rate } -> std::convertible_to<Duration>;
};

template <typename... Ts> constexpr bool validate_weapons(std::variant<Ts...> *) {
    return (is_valid_weapon<Ts> && ...);
}

class Weapon {
  private:
    using WeaponVariant = std::variant<Bow, Sniper, Sword>;
    WeaponVariant m_type;

    static_assert(validate_weapons(static_cast<WeaponVariant *>(nullptr)),
                  "All weapon variants are not valid.");

    template <typename T> bool holds() const { return std::holds_alternative<T>(m_type); }
    template <typename T> T &get() { return std::get<T>(m_type); }

    template <typename T>
    constexpr Weapon(std::in_place_type_t<T>)
        : m_type(std::in_place_type<T>), damage(T::damage), damage_type(T::damage_type),
          range(T::range), fire_rate(T::fire_rate) {}

    TimePoint m_last_fire = TimePoint(Duration::zero());

  public:
    float damage;
    DamageType damage_type;
    float range;
    Duration fire_rate;

    template <typename T> static constexpr Weapon create_weapon() {
        return Weapon(std::in_place_type<T>);
    }

    bool can_fire() const { return Clock::now() - m_last_fire >= fire_rate; }
    void fire() { m_last_fire = Clock::now(); }
};
