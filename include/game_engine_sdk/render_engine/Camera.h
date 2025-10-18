#pragma once

#include "glm/ext/matrix_transform.hpp"

class Camera2D {
  private:
    glm::vec2 m_position;
    float m_rotation;
    float m_zoom;

    float m_viewport_width;
    float m_viewport_height;

    glm::mat4 m_transform_matrix;
    glm::mat4 m_projection_matrix;

    void update_transform_matrix();
    void update_projection_matrix();

  public:
    // TODO: I WILL 100% FALL INTO THE TRAP WHERE I MIX PIXEL COORDINATES AND NORMALIZED
    // COORDINATES
    Camera2D();
    Camera2D(float viewport_width, float viewport_height);
    ~Camera2D() = default;

    void set_position(glm::vec2 &new_pos);
    void set_position(glm::vec2 &&new_pos);
    void set_relative_position(glm::vec2 &delta);
    void set_relative_position(glm::vec2 &&delta);

    void set_rotation(float new_rot);
    void set_relative_rotation(float delta);

    void set_zoom(float new_zoom);
    void set_relative_zoom(float delta);

    glm::mat4 get_transform_matrix();
    glm::mat4 get_projection_matrix();

    glm::mat4 get_transform_projection_matrix();

    static glm::mat4 get_default_transform_matrix();
};
