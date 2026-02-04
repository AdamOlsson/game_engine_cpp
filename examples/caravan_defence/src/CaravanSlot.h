#pragma once

#include "Guard.h"
#include "camera/Camera.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "math/Matrix.h"
#include "math/shape.h"
#include "util/colors.h"

class Guard;

class CaravanSlot {
  private:
    math::Matrix m_model_matrix;
    static constexpr util::colors::Color m_color =
        util::colors::rgba(0.5f, 0.5f, 0.5f, 0.2f);
    static constexpr util::colors::Color m_highlighted_color =
        util::colors::rgba(0.5f, 0.5f, 0.5f, 0.8f);
    graphics_pipeline::quad::QuadPipelineSBO *m_render_data;

    bool m_is_highlighted = false;
    bool m_is_visible = true;

    Guard *m_occupying_guard = nullptr;

  public:
    static constexpr float width = 50.0f;
    static constexpr float height = 50.0f;

    CaravanSlot() = default;
    CaravanSlot(const camera::WorldPoint2D &position)
        : m_model_matrix(math::Matrix().translate(position).scale(width, height, 1.0f)) {}
    CaravanSlot(CaravanSlot &&other) noexcept = default;
    CaravanSlot(const CaravanSlot &other) = default;
    CaravanSlot &operator=(const CaravanSlot &other) = default;
    CaravanSlot &operator=(CaravanSlot &&other) noexcept = default;
    ~CaravanSlot() {}

    bool is_point_inside(const camera::WorldPoint2D &point) {
        // World position with no regard to the world grid
        const camera::WorldPoint2D position = m_model_matrix.position_2d();
        return math::is_point_inside_rectangle(point, position, width, height);
    }

    void set_render_data(graphics_pipeline::quad::QuadPipelineSBO *render_data) {
        if (render_data == nullptr) {
            return;
        }
        m_render_data = render_data;
        m_render_data->model_matrix = m_model_matrix;
        m_render_data->color = m_color;
    }

    void set_highlighted(const bool is_highlighted) {
        m_is_highlighted = is_highlighted;
        m_render_data->color = m_is_highlighted ? m_highlighted_color : m_color;
    }

    void toggle_selected() {
        m_is_highlighted = !m_is_highlighted;
        m_render_data->color = m_is_highlighted ? m_highlighted_color : m_color;
    }

    bool is_visible() const { return m_is_visible; }

    void set_visibility(const bool is_visible) {
        m_is_visible = is_visible;
        m_render_data->color = m_is_visible ? m_color : util::colors::TRANSPARENT;
    }

    void toggle_visibility() {
        m_is_visible = !m_is_visible;
        m_render_data->color = m_is_visible ? m_color : util::colors::TRANSPARENT;
    }

    camera::WorldPoint2D get_world_position() const {
        return m_model_matrix.position_2d();
    }

    void set_occupying_guard(Guard *guard) {
        DEBUG_ASSERT(guard != nullptr, "Error: Setting guard slot with nullptr, use "
                                       "clear_occupying_guard() to clear the slot.");
        set_visibility(false);
        set_highlighted(false);
        m_occupying_guard = guard;
    }

    void clear_occupying_guard() {
        set_visibility(true);
        set_highlighted(false);
        m_occupying_guard = nullptr;
    }

    Guard *get_occupying_guard() { return m_occupying_guard; }
    bool is_occupied() const { return m_occupying_guard != nullptr; }
    bool is_free() const { return m_occupying_guard == nullptr; }
};
