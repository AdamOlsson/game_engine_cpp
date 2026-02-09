#pragma once

#include "camera/Camera.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "math/Matrix.h"
#include "math/shape.h"
#include "util/colors.h"

class Caravan {
  private:
    math::Matrix m_model_matrix;
    static constexpr util::colors::Color m_color = util::colors::MAGENTA;
    graphics_pipeline::quad::QuadPipelineSBO *m_render_data = nullptr;

    static constexpr glm::vec2 m_size = glm::vec2(100.0f, 200.0f);

  public:
    Caravan() = default;
    Caravan(const camera::WorldPoint2D &position)
        : m_model_matrix(
              math::Matrix().translate(position).scale(m_size.x, m_size.y, 1.0f)) {}

    Caravan(Caravan &&other) noexcept = default;
    Caravan(const Caravan &other) = default;
    Caravan &operator=(const Caravan &other) = default;
    Caravan &operator=(Caravan &&other) noexcept = default;
    ~Caravan() {}

    void set_render_data(graphics_pipeline::quad::QuadPipelineSBO *render_data) {
        if (render_data == nullptr) {
            return;
        }
        m_render_data = render_data;
        m_render_data->model_matrix = m_model_matrix;
        m_render_data->color = m_color;
    }

    bool is_point_inside(const camera::WorldPoint2D &point) {
        // World position with no regard to the world grid
        const camera::WorldPoint2D position = m_model_matrix.position_2d();
        return math::is_point_inside_rectangle(point, position, m_size.x, m_size.y);
    }

    camera::WorldPoint2D get_world_position() const {
        return m_model_matrix.position_2d();
    }
};
