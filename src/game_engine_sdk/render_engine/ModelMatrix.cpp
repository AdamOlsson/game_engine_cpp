#include "game_engine_sdk/render_engine/ModelMatrix.h"

ModelMatrix::ModelMatrix()
    : m_matrix(glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f))) {}

ModelMatrix::ModelMatrix(const glm::mat4 &mat) : m_matrix(mat) {}

ModelMatrix::ModelMatrix(const ModelMatrix &other) : m_matrix(other.m_matrix) {}

ModelMatrix &ModelMatrix::translate(const glm::vec3 &vec) {
    m_matrix = glm::translate(m_matrix, vec);
    return *this;
}

ModelMatrix &ModelMatrix::scale(const float x, const float y, const float z) {
    m_matrix = glm::scale(m_matrix, glm::vec3(x, y, z));
    return *this;
}

ModelMatrix &ModelMatrix::scale(const glm::vec3 &vec) {
    m_matrix = glm::scale(m_matrix, vec);
    return *this;
}

ModelMatrix &ModelMatrix::scale(const glm::vec3 &&vec) {
    m_matrix = glm::scale(m_matrix, vec);
    return *this;
}

ModelMatrix &ModelMatrix::rotate(const float angle, const glm::vec3 &axis) {
    m_matrix = glm::rotate(m_matrix, angle, axis);
    return *this;
}
