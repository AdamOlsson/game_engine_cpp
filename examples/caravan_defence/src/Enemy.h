#pragma once

#include "camera/Camera.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "math/Matrix.h"
#include "math/shape.h"
#include "util/colors.h"

class Enemy {
  private:
    // Render data
    math::Matrix m_model_matrix;
    static constexpr util::colors::Color m_color = util::colors::RED;
    graphics_pipeline::quad::QuadPipelineSBO *m_render_data;

    static constexpr glm::vec2 m_size = glm::vec2(50.0f, 50.0f);
    static constexpr glm::vec2 m_velocity = glm::vec2(25.0f, 25.0f);

  public:
    static constexpr size_t spawn_rate_ms = 1000;

    Enemy() = default;
    Enemy(const camera::WorldPoint2D &position)
        : m_model_matrix(
              math::Matrix().translate(position).scale(m_size.x, m_size.y, 1.0f)) {}
    ~Enemy() {}

    Enemy(Enemy &&other) noexcept = default;
    Enemy(const Enemy &other) = default;
    Enemy &operator=(const Enemy &other) = default;
    Enemy &operator=(Enemy &&other) noexcept = default;

    bool is_point_inside(const camera::WorldPoint2D &point) {
        // World position with no regard to the world grid
        const camera::WorldPoint2D position = m_model_matrix.position_2d();
        return math::is_point_inside_rectangle(point, position, m_size.x, m_size.y);
    }

    void set_render_data(graphics_pipeline::quad::QuadPipelineSBO *render_data) {
        if (render_data == nullptr) {
            return;
        }
        m_render_data = render_data;
        m_render_data->model_matrix = m_model_matrix;
        m_render_data->color = m_color;
    }

    void set_world_position(const camera::WorldPoint2D &position) {
        m_model_matrix =
            math::Matrix().translate(position).scale(m_size.x, m_size.y, 1.0f);
        m_render_data->model_matrix = m_model_matrix;
    }

    camera::WorldPoint2D get_world_position() const {
        return m_model_matrix.position_2d();
    }
};
