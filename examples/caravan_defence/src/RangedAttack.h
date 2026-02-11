#pragma once

#include "camera/Camera.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "math/Matrix.h"
#include "math/equations.h"
class RangedAttack {
  private:
    // Render data
    math::Matrix m_model_matrix;
    static constexpr util::colors::Color m_color = util::colors::ORANGE;

    graphics_pipeline::quad::QuadSBOHandle m_render_data_handle;

    float m_width;
    static constexpr float m_height = 10.0f;

    float m_lifetime_count = 0.0f;
    static constexpr float m_lifetime_ms = 1000.0f;

    bool m_is_visible = true;

  public:
    RangedAttack(const camera::WorldPoint2D &start, const camera::WorldPoint2D &end)
        : m_width(math::distance(start, end)) {
        const camera::WorldPoint2D local_vec = end - start;
        const float rotation = math::angle_to_x_axis(local_vec);
        m_model_matrix = math::Matrix()
                             .translate(start + local_vec / 2.0f)
                             .rotate_z(rotation)
                             .scale(m_width, m_height, 0.0f);
    }

    RangedAttack(RangedAttack &&other) noexcept = default;
    RangedAttack &operator=(RangedAttack &&other) noexcept = default;

    // Delete copy constructor and operator
    RangedAttack(const RangedAttack &other) = delete;
    RangedAttack &operator=(const RangedAttack &other) = delete;

    ~RangedAttack() { remove(); }

    void remove() { m_render_data_handle.return_to_source(); }

    void set_render_data(graphics_pipeline::quad::QuadSBOHandle &&render_data_handle) {
        m_render_data_handle = std::move(render_data_handle);
        m_render_data_handle.data->model_matrix = m_model_matrix;
        m_render_data_handle.data->color = m_color;
    }

    bool is_visible() { return m_is_visible; }

    void update(const float dt_s) {
        /*DEBUG_ASSERT( m_render_data != nullptr,*/
        /*    "Error: Trying call update() RangedAttack when render data is a
         * nullptr.");*/

        if (m_lifetime_count > m_lifetime_ms) {
            m_is_visible = false;
        }

        m_lifetime_count += dt_s * 1000.0f;
    }
};
