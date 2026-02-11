#pragma once

#include "camera/Camera.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "math/Matrix.h"
#include "math/interpolate.h"
#include "math/shape.h"
#include "util/colors.h"

class Enemy {
  private:
    // Render data
    math::Matrix m_model_matrix;
    static constexpr util::colors::Color m_color = util::colors::RED;
    /*graphics_pipeline::quad::QuadPipelineSBO *m_render_data = nullptr;*/
    graphics_pipeline::quad::QuadSBOHandle m_render_data_handle;

    static constexpr glm::vec2 m_size = glm::vec2(50.0f, 50.0f);
    static constexpr float m_velocity = 25.0f;

  public:
    static constexpr size_t spawn_rate_ms = 1000;

    Enemy() = default;
    Enemy(const camera::WorldPoint2D &position)
        : m_model_matrix(
              math::Matrix().translate(position).scale(m_size.x, m_size.y, 1.0f)) {}

    ~Enemy() { die(); }

    Enemy(Enemy &&other) noexcept = default;
    Enemy &operator=(Enemy &&other) noexcept = default;

    // Delete copy constructor and operator
    Enemy(const Enemy &other) = delete;
    Enemy &operator=(const Enemy &other) = delete;

    bool is_point_inside(const camera::WorldPoint2D &point) {
        // World position with no regard to the world grid
        const camera::WorldPoint2D position = m_model_matrix.position_2d();
        return math::is_point_inside_rectangle(point, position, m_size.x, m_size.y);
    }

    void set_render_data(graphics_pipeline::quad::QuadSBOHandle &&render_data_handle) {
        m_render_data_handle = std::move(render_data_handle);
        m_render_data_handle.data->model_matrix = m_model_matrix;
        m_render_data_handle.data->color = m_color;
    }

    void set_world_position(const camera::WorldPoint2D &position) {
        m_model_matrix =
            math::Matrix().translate(position).scale(m_size.x, m_size.y, 1.0f);
        m_render_data_handle.data->model_matrix = m_model_matrix;
    }

    camera::WorldPoint2D get_world_position() const {
        return m_model_matrix.position_2d();
    }

    void move_towards(const camera::WorldPoint2D &target, const float dt) {
        const camera::WorldPoint2D position = m_model_matrix.position_2d();
        const float distance = math::distance(position, target);
        const float movement = m_velocity * dt;

        const float fraction = std::min(movement / distance, 1.0f);
        camera::WorldPoint2D new_position = math::lerp(position, target, fraction);

        set_world_position(new_position);
    }

    void die() { m_render_data_handle.return_to_source(); }
};
