#include "game_engine_sdk/render_engine/Camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include <utility>

Camera2D::Camera2D()
    : m_position(glm::vec2(0.0f)), m_rotation(0.0f), m_zoom(1.0f), m_viewport_width(1.0f),
      m_viewport_height(1.0f), m_transform_matrix(glm::mat4(1.0f)),
      m_projection_matrix(glm::mat4(1.0f)) {}

Camera2D::Camera2D(float viewport_width, float viewport_height)
    : m_position(glm::vec2(0.0f)), m_rotation(0.0f), m_zoom(1.0f),
      m_viewport_width(viewport_width), m_viewport_height(viewport_height) {}

void Camera2D::set_position(glm::vec2 &new_pos) { m_position = new_pos; }
void Camera2D::set_position(glm::vec2 &&new_pos) { m_position = std::move(new_pos); }
void Camera2D::set_relative_position(glm::vec2 &delta) { m_position += delta; }
void Camera2D::set_relative_position(glm::vec2 &&delta) { m_position += delta; }

void Camera2D::set_rotation(float new_rot) { m_rotation = new_rot; }
void Camera2D::set_relative_rotation(float delta) { m_rotation += delta; }

void Camera2D::set_zoom(float new_zoom) { m_zoom = new_zoom; }
void Camera2D::set_relative_zoom(float delta) { m_zoom += delta; }

void Camera2D::update_transform_matrix() {
    m_transform_matrix = glm::mat4(1.0f);
    m_transform_matrix =
        glm::rotate(m_transform_matrix, -m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    m_transform_matrix =
        glm::translate(m_transform_matrix, glm::vec3(-m_position.x, m_position.y, 0.0f));
}

glm::mat4 Camera2D::get_transform_matrix() {
    update_transform_matrix();
    return m_transform_matrix;
}

glm::mat4 Camera2D::get_default_transform_matrix() { return glm::mat4(1.0f); }

void Camera2D::update_projection_matrix() {
    float aspect_ratio = m_viewport_width / m_viewport_height;
    float orthogonal_height = 2.0f / m_zoom; // Camera height
    float orthogonal_width = orthogonal_height * aspect_ratio;
    m_projection_matrix =
        glm::ortho(-orthogonal_width / 2.0f, orthogonal_width / 2.0f,
                   -orthogonal_height / 2.0f, orthogonal_height / 2.0f, -1.0f, 1.0f);
}

glm::mat4 Camera2D::get_projection_matrix() {
    update_projection_matrix();
    return m_projection_matrix;
}

glm::mat4 Camera2D::get_transform_projection_matrix() {
    update_transform_matrix();
    update_projection_matrix();
    return m_projection_matrix * m_transform_matrix;
}
