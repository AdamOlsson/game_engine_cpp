#include "game_engine_sdk/render_engine/Camera.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <utility>

Camera2D::Camera2D()
    : m_position(glm::vec2(0.0f)), m_rotation(0.0f), m_zoom(1.0f), m_viewport_width(1.0f),
      m_viewport_height(1.0f), m_view_matrix(glm::mat4(1.0f)), m_base_orho_height(2.0f),
      m_projection_matrix(glm::mat4(1.0f)) {}

Camera2D::Camera2D(const float viewport_width, const float viewport_height,
                   const float base_ortho_height)
    : m_position(glm::vec2(0.0f)), m_rotation(0.0f), m_zoom(1.0f),
      m_viewport_width(viewport_width), m_viewport_height(viewport_height),
      m_base_orho_height(base_ortho_height) {}

void Camera2D::set_position(WorldPoint2D &new_pos) { m_position = new_pos; }
void Camera2D::set_position(WorldPoint2D &&new_pos) { m_position = std::move(new_pos); }
void Camera2D::set_relative_position(WorldPoint2D &delta) {
    m_position = m_position + delta;
}
void Camera2D::set_relative_position(WorldPoint2D &&delta) {
    m_position = m_position + delta;
}

void Camera2D::set_rotation(float new_rot) { m_rotation = new_rot; }
void Camera2D::set_relative_rotation(float delta) { m_rotation += delta; }

void Camera2D::set_zoom(float new_zoom) { m_zoom = new_zoom; }
void Camera2D::set_relative_zoom(float delta) { m_zoom += delta; }

void Camera2D::update_view_matrix() {
    m_view_matrix = glm::mat4(1.0f);
    m_view_matrix = glm::rotate(m_view_matrix, -m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    m_view_matrix =
        glm::translate(m_view_matrix, glm::vec3(m_position.x, -m_position.y, 0.0f));
}

glm::mat4 Camera2D::get_view_matrix() {
    update_view_matrix();
    return m_view_matrix;
}

glm::mat4 Camera2D::get_default_view_matrix() { return glm::mat4(1.0f); }

void Camera2D::update_projection_matrix() {
    float aspect_ratio = m_viewport_width / m_viewport_height;
    float orthogonal_height = m_base_orho_height / m_zoom; // Camera height
    float orthogonal_width = orthogonal_height * aspect_ratio;
    m_projection_matrix =
        glm::ortho(-orthogonal_width / 2.0f, orthogonal_width / 2.0f,
                   -orthogonal_height / 2.0f, orthogonal_height / 2.0f, -1.0f, 1.0f);
}

WorldPoint2D
Camera2D::viewport_delta_to_world(const window::ViewportPoint &&viewport_delta) const {
    float aspect = m_viewport_width / m_viewport_height;
    float ortho_height = m_base_orho_height / m_zoom;
    float ortho_width = ortho_height * aspect;

    float world_per_pixel_x = ortho_width / m_viewport_width;
    float world_per_pixel_y = ortho_height / m_viewport_height;

    return glm::vec2(viewport_delta.x * world_per_pixel_x,
                     viewport_delta.y * world_per_pixel_y);
}

WorldPoint2D
Camera2D::viewport_delta_to_world(const window::ViewportPoint &viewport_delta) const {
    float aspect = m_viewport_width / m_viewport_height;
    float ortho_height = m_base_orho_height / m_zoom;
    float ortho_width = ortho_height * aspect;

    float world_per_pixel_x = ortho_width / m_viewport_width;
    float world_per_pixel_y = ortho_height / m_viewport_height;

    return glm::vec2(viewport_delta.x * world_per_pixel_x,
                     viewport_delta.y * world_per_pixel_y);
}

WorldPoint2D
Camera2D::viewport_to_world(const window::ViewportPoint &viewport_pos) const {
    const auto normalized = glm::vec2(viewport_pos.x / m_viewport_width / 2.0f,
                                      viewport_pos.y / m_viewport_height / 2.0f);
    auto world_position = glm::inverse(m_projection_matrix * m_view_matrix) *
                          glm::vec4(normalized, 0.0f, 1.0f);
    return glm::vec2(world_position.x, world_position.y);
}

glm::mat4 Camera2D::get_projection_matrix() {
    update_projection_matrix();
    return m_projection_matrix;
}

uint32_t Camera2D::matrix_size() { return sizeof(glm::mat4); }

glm::mat4 Camera2D::get_view_projection_matrix() {
    update_view_matrix();
    update_projection_matrix();
    return m_projection_matrix * m_view_matrix;
}
