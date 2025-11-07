#include "camera/Camera.h"
// clang-format off
#include "util/io.h"
#include "logger/logger.h"
// clang-format on
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <utility>

camera::Camera2D::Camera2D()
    : m_position(glm::vec2(0.0f)), m_rotation(0.0f), m_zoom(1.0f), m_viewport_width(1.0f),
      m_viewport_height(1.0f), m_view_matrix(glm::mat4(1.0f)), m_base_orho_height(2.0f),
      m_projection_matrix(glm::mat4(1.0f)) {
    update_view_matrix();
    update_projection_matrix();
}

camera::Camera2D::Camera2D(const float viewport_width, const float viewport_height,
                           const float base_ortho_height)
    : m_position(glm::vec2(0.0f)), m_rotation(0.0f), m_zoom(1.0f),
      m_viewport_width(viewport_width), m_viewport_height(viewport_height),
      m_base_orho_height(base_ortho_height) {
    update_view_matrix();
    update_projection_matrix();
}

void camera::Camera2D::set_position(const WorldPoint2D &new_pos) { m_position = new_pos; }
void camera::Camera2D::set_position(WorldPoint2D &&new_pos) {
    m_position = std::move(new_pos);
}
void camera::Camera2D::set_relative_position(const WorldPoint2D &delta) {
    m_position = m_position + delta;
}

void camera::Camera2D::set_relative_position(WorldPoint2D &&delta) {
    m_position = m_position + delta;
}

void camera::Camera2D::set_rotation(const float new_rot) { m_rotation = new_rot; }

void camera::Camera2D::set_relative_rotation(const float delta) { m_rotation += delta; }

void camera::Camera2D::set_zoom(const float new_zoom, const ViewportPoint &zoom_target) {
    m_zoom = glm::clamp(new_zoom, 0.1f, 10.0f);
}

void camera::Camera2D::set_relative_zoom(const float delta,
                                         const ViewportPoint &zoom_target) {

    const auto world_pos_before_zoom = viewport_to_world(zoom_target);
    float old_zoom = m_zoom;
    m_zoom = glm::clamp(m_zoom + delta, 0.1f, 10.0f);

    if (old_zoom == m_zoom) {
        return;
    }

    update_projection_matrix();

    const auto world_pos_after_zoom = viewport_to_world(zoom_target);
    const auto offset = world_pos_before_zoom - world_pos_after_zoom;

    logger::debug("zoom target viewport pos: ", world_pos_before_zoom);
    logger::debug("zoom target world pos before zoom: ", world_pos_before_zoom);
    logger::debug("zoom target world pos after zoom: ", world_pos_after_zoom);
    logger::debug("camera old position: ", m_position);
    logger::debug("camera new position: ", m_position + offset);
    logger::debug("camera position offset: ", offset);
    logger::debug("camera zoom: ", m_zoom);
    logger::debug("");
    m_position += offset;
    update_view_matrix();
}

void camera::Camera2D::update_view_matrix() {
    m_view_matrix = glm::mat4(1.0f);
    m_view_matrix = glm::rotate(m_view_matrix, -m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    m_view_matrix =
        glm::translate(m_view_matrix, glm::vec3(-m_position.x, -m_position.y, 0.0f));
}

glm::mat4 camera::Camera2D::get_view_matrix() {
    update_view_matrix();
    return m_view_matrix;
}

glm::mat4 camera::Camera2D::get_default_view_matrix() { return glm::mat4(1.0f); }

void camera::Camera2D::update_projection_matrix() {
    float aspect_ratio = m_viewport_width / m_viewport_height;
    float orthogonal_height = m_base_orho_height / m_zoom; // Camera height
    float orthogonal_width = orthogonal_height * aspect_ratio;
    m_projection_matrix =
        glm::ortho(-orthogonal_width / 2.0f, orthogonal_width / 2.0f,
                   -orthogonal_height / 2.0f, orthogonal_height / 2.0f, -1.0f, 1.0f);
}

camera::WorldPoint2D
camera::Camera2D::viewport_delta_to_world(const ViewportPoint &&viewport_delta) const {
    float aspect = m_viewport_width / m_viewport_height;
    float ortho_height = m_base_orho_height / m_zoom;
    float ortho_width = ortho_height * aspect;

    float world_per_pixel_x = ortho_width / m_viewport_width;
    float world_per_pixel_y = ortho_height / m_viewport_height;

    return glm::vec2(viewport_delta.x * world_per_pixel_x,
                     viewport_delta.y * world_per_pixel_y);
}

camera::WorldPoint2D
camera::Camera2D::viewport_delta_to_world(const ViewportPoint &viewport_delta) const {
    float aspect = m_viewport_width / m_viewport_height;
    float ortho_height = m_base_orho_height / m_zoom;
    float ortho_width = ortho_height * aspect;

    float world_per_pixel_x = ortho_width / m_viewport_width;
    float world_per_pixel_y = ortho_height / m_viewport_height;

    return glm::vec2(viewport_delta.x * world_per_pixel_x,
                     viewport_delta.y * world_per_pixel_y);
}

camera::WorldPoint2D
camera::Camera2D::viewport_to_world(const ViewportPoint &viewport_pos) const {
    const auto normalized = glm::vec2(viewport_pos.x / (m_viewport_width / 2.0f),
                                      viewport_pos.y / (m_viewport_height / 2.0f));
    const auto world_position = glm::inverse(m_projection_matrix * m_view_matrix) *
                                glm::vec4(normalized, 0.0f, 1.0f);

    return glm::vec2(world_position.x, world_position.y);
}

glm::mat4 camera::Camera2D::get_projection_matrix() {
    update_projection_matrix();
    return m_projection_matrix;
}

uint32_t camera::Camera2D::matrix_size() { return sizeof(glm::mat4); }

glm::mat4 camera::Camera2D::get_view_projection_matrix() {
    update_view_matrix();
    update_projection_matrix();
    return m_projection_matrix * m_view_matrix;
}

glm::vec2 camera::Camera2D::get_position() { return m_position; }
float camera::Camera2D::get_rotation() { return m_rotation; }
float camera::Camera2D::get_zoom() { return m_zoom; }
