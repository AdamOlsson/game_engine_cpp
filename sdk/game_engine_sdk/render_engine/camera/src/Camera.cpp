#include "camera/Camera.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <utility>

#ifndef NDEBUG
#include <cstdlib>

#define DEBUG_ASSERT(expr)                                                               \
    do {                                                                                 \
        if (!(expr)) {                                                                   \
            std::cerr << "Assertion failed: (" #expr "), " << "function " << __func__    \
                      << ", file " << __FILE__ << ", line " << __LINE__ << std::endl;    \
            std::abort();                                                                \
        }                                                                                \
    } while (false)
#else
#define DEBUG_ASSERT(expr) ((void)0)
#endif

camera::Camera2D::Camera2D()
    : m_position(glm::vec2(0.0f)), m_rotation(0.0f), m_zoom(1.0f), m_max_zoom(10.0f),
      m_min_zoom(0.1f), m_viewport_width(1.0f), m_viewport_height(1.0f),
      m_view_matrix(glm::mat4(1.0f)), m_base_orho_height(2.0f),
      m_projection_matrix(glm::mat4(1.0f)) {
    update_view_matrix();
    update_projection_matrix();
}

camera::Camera2D::Camera2D(const float viewport_width, const float viewport_height,
                           const float base_ortho_height)
    : m_position(glm::vec2(0.0f)), m_rotation(0.0f), m_zoom(1.0f), m_max_zoom(10.0f),
      m_min_zoom(0.1f), m_viewport_width(viewport_width),
      m_viewport_height(viewport_height), m_base_orho_height(base_ortho_height) {
    update_view_matrix();
    update_projection_matrix();
}

void camera::Camera2D::set_position(const float x, const float y) {
    m_position = glm::vec2(x, y);
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

void camera::Camera2D::set_zoom(const float new_zoom) {
    m_zoom = glm::clamp(new_zoom, m_min_zoom, m_max_zoom);
}

void camera::Camera2D::set_relative_zoom(const float delta) {
    m_zoom = glm::clamp(m_zoom + delta, m_min_zoom, m_max_zoom);
}

void camera::Camera2D::configure_max_zoom(const float max) { m_max_zoom = max; }
void camera::Camera2D::configure_min_zoom(const float min) { m_min_zoom = min; }

void camera::Camera2D::update_view_matrix() {

    float pixel_size = (m_base_orho_height / m_zoom) / m_viewport_height;

    // Snap camera position to pixel boundaries
    glm::vec2 snapped_position;
    snapped_position.x = std::round(m_position.x / pixel_size) * pixel_size;
    snapped_position.y = std::round(m_position.y / pixel_size) * pixel_size;

    m_view_matrix = glm::mat4(1.0f);
    m_view_matrix = glm::rotate(m_view_matrix, -m_rotation, glm::vec3(0.0f, 0.0f, 1.0f));
    m_view_matrix = glm::translate(
        m_view_matrix, glm::vec3(-snapped_position.x, -snapped_position.y, 0.0f));
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
    float aspect = m_viewport_width / m_viewport_height;
    float ortho_height = m_base_orho_height / m_zoom;
    float ortho_width = ortho_height * aspect;

    float world_per_pixel_x = ortho_width / m_viewport_width;
    float world_per_pixel_y = ortho_height / m_viewport_height;

    glm::vec2 world_offset =
        glm::vec2(viewport_pos.x * world_per_pixel_x, viewport_pos.y * world_per_pixel_y);

    glm::vec2 world_pos = m_position + world_offset;

    return world_pos;
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

camera::WorldPoint2D camera::Camera2D::get_position() { return m_position; }
float camera::Camera2D::get_rotation() { return m_rotation; }
float camera::Camera2D::get_zoom() { return m_zoom; }
