#include "math/Matrix.h"

math::Matrix::Matrix()
    : m_matrix(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f))) {}

math::Matrix::Matrix(const glm::mat4 &mat) : m_matrix(mat) {}

/*math::Matrix::Matrix(const Matrix &other) : m_matrix(other.m_matrix) {}*/

/*math::Matrix::Matrix(Matrix &&other) noexcept : m_matrix(std::move(other)) {};*/

math::Matrix &math::Matrix::translate(const glm::vec2 &vec) {
    m_matrix = glm::translate(m_matrix, glm::vec3(vec.x, vec.y, 0.0f));
    return *this;
}

math::Matrix &math::Matrix::translate(const glm::vec3 &vec) {
    m_matrix = glm::translate(m_matrix, vec);
    return *this;
}

math::Matrix &math::Matrix::scale(const float x, const float y, const float z) {
    m_matrix = glm::scale(m_matrix, glm::vec3(x, y, z));
    return *this;
}

math::Matrix &math::Matrix::scale(const glm::vec3 &vec) {
    m_matrix = glm::scale(m_matrix, vec);
    return *this;
}

math::Matrix &math::Matrix::scale(const glm::vec3 &&vec) {
    m_matrix = glm::scale(m_matrix, vec);
    return *this;
}

math::Matrix &math::Matrix::rotate(const float radians, const glm::vec3 &axis) {
    m_matrix = glm::rotate(m_matrix, radians, axis);
    return *this;
}

math::Matrix &math::Matrix::rotate_z(const float radians) {
    m_matrix = glm::rotate(m_matrix, radians, glm::vec3(0.0f, 0.0f, 1.0f));
    return *this;
}
