#include "math/Matrix.h"

math::Matrix::Matrix()
    : m_matrix(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f))) {}

math::Matrix::Matrix(const glm::mat4 &mat) : m_matrix(mat) {}

math::Matrix::Matrix(const Matrix &other) : m_matrix(other.m_matrix) {}

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

math::Matrix &math::Matrix::rotate(const float angle, const glm::vec3 &axis) {
    m_matrix = glm::rotate(m_matrix, angle, axis);
    return *this;
}
