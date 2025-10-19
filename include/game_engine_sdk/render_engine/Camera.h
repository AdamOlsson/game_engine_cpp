#pragma once

#include "glm/glm.hpp"

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

    void set_position(glm::vec2 &new_pos);
    void set_position(glm::vec2 &&new_pos);

    void set_relative_position(glm::vec2 &delta);
    void set_relative_position(glm::vec2 &&delta);

    void set_rotation(float new_rot);
    void set_relative_rotation(float delta);

    void set_zoom(float new_zoom);
    void set_relative_zoom(float delta);

    glm::mat4 get_view_matrix();
    glm::mat4 get_projection_matrix();
    glm::mat4 get_view_projection_matrix();

    static uint32_t matrix_size();

    static glm::mat4 get_default_view_matrix();
};
