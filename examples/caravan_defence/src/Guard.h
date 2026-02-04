#pragma once

#include "CaravanSlot.h"
#include "camera/Camera.h"
#include "graphics_pipeline/quad/QuadPipelineSBO.h"
#include "math/Matrix.h"
#include "util/colors.h"

class CaravanSlot;

class Guard {
  private:
    // Render data
    math::Matrix m_model_matrix;
    static constexpr util::colors::Color m_color = util::colors::GREEN;
    static constexpr util::colors::Color m_selected_color = util::colors::YELLOW;
    graphics_pipeline::quad::QuadPipelineSBO *m_render_data;

    bool m_is_selected = false;
    CaravanSlot *m_caravan_slot = nullptr;

    static constexpr glm::vec2 m_size = glm::vec2(50.0f, 50.0f);

  public:
    Guard() = default;
    Guard(CaravanSlot *slot);
    ~Guard() {}

    Guard(Guard &&other) noexcept = default;
    Guard(const Guard &other) = default;
    Guard &operator=(const Guard &other) = default;
    Guard &operator=(Guard &&other) noexcept = default;

    bool is_point_inside(const camera::WorldPoint2D &point);

    void set_render_data(graphics_pipeline::quad::QuadPipelineSBO *render_data);

    void set_selected(const bool is_selected);
    void toggle_selected();

    void set_world_position(const camera::WorldPoint2D &position);
    camera::WorldPoint2D get_world_position() const;

    void set_caravan_slot(CaravanSlot *slot);
    CaravanSlot *get_caravan_slot() const;
};
