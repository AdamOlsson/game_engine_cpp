#pragma once

#include "camera/Camera.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "math/Matrix.h"
#include "util/colors.h"

class Caravan {
  private:
    math::Matrix m_model_matrix;
    static constexpr util::colors::Color m_color = util::colors::MAGENTA;
    graphics_pipeline::quad::QuadPipelineSBO *m_render_data;

  public:
    static constexpr float width = 100.0f;
    static constexpr float height = 200.0f;

    Caravan() = default;
    Caravan(const camera::WorldPoint2D &position)
        : m_model_matrix(math::Matrix().translate(position).scale(width, height, 1.0f)) {}
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
};
