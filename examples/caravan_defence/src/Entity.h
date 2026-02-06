#pragma once
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "math/Matrix.h"
#include "math/Vector2.h"
#include <variant>
namespace entity {

struct Caravan {
    static constexpr util::colors::Color color = util::colors::MAGENTA;
    static constexpr math::Vector2 size = math::Vector2(100.0f, 200.0f);
};

struct CaravanSlot {
    static constexpr util::colors::Color color =
        util::colors::rgba(0.5f, 0.5f, 0.5f, 0.2f);
    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);
};

struct Enemy {
    static constexpr util::colors::Color color = util::colors::RED;
    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);
};

struct Guard {
    static constexpr util::colors::Color color = util::colors::GREEN;
    static constexpr math::Vector2 size = math::Vector2(50.0f, 50.0f);
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
    using EntityVariant = std::variant<Caravan, CaravanSlot, Enemy, Guard>;
    EntityVariant m_type;

    math::Matrix m_model_matrix;

    graphics_pipeline::quad::QuadPipelineSBO *m_render_data = nullptr;

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
    void set_render_data(graphics_pipeline::quad::QuadPipelineSBO *render_data) {
        if (render_data == nullptr) {
            return;
        }
        m_render_data = render_data;
        m_render_data->model_matrix = m_model_matrix;
        m_render_data->color = get_color();
    }
};

} // namespace entity
