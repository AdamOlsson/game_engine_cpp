#pragma once

#include "game_engine_sdk/WorldPoint.h"
#include "glm/glm.hpp"
#include "window/ViewportPoint.h"

class Camera2D {
  private:
    glm::vec2 m_position;
    float m_rotation;
    float m_zoom;

    float m_viewport_width;
    float m_viewport_height;
    float m_base_orho_height;

    glm::mat4 m_view_matrix;
    glm::mat4 m_projection_matrix;

    void update_view_matrix();
    void update_projection_matrix();

  public:
    Camera2D();
    Camera2D(const float viewport_width, const float viewport_height,
             const float base_ortho_height);
    ~Camera2D() = default;

    void set_position(WorldPoint2D &new_pos);
    void set_position(WorldPoint2D &&new_pos);

    void set_relative_position(WorldPoint2D &delta);
    void set_relative_position(WorldPoint2D &&delta);

    void set_rotation(const float new_rot);
    void set_relative_rotation(const float delta);

    void set_zoom(const float new_zoom, const window::ViewportPoint &zoom_target);
    void set_relative_zoom(const float delta, const window::ViewportPoint &zoom_target);

    glm::mat4 get_view_matrix();
    glm::mat4 get_projection_matrix();
    glm::mat4 get_view_projection_matrix();

    glm::vec2 get_position();
    float get_rotation();
    float get_zoom();

    WorldPoint2D
    viewport_delta_to_world(const window::ViewportPoint &viewport_delta) const;
    WorldPoint2D
    viewport_delta_to_world(const window::ViewportPoint &&viewport_delta) const;
    WorldPoint2D viewport_to_world(const window::ViewportPoint &viewport_pos) const;

    static uint32_t matrix_size();

    static glm::mat4 get_default_view_matrix();
};
