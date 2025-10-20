#include "game_engine_sdk/render_engine/ModelMatrix.h"
#include "glm/ext/matrix_transform.hpp"

ModelMatrix::ModelMatrix() : m_matrix(glm::mat4(1.0f)) {}

ModelMatrix::ModelMatrix(const glm::mat4 &mat) : m_matrix(mat) {}

void ModelMatrix::translate(const glm::vec3 &vec) {
    m_matrix = glm::translate(m_matrix, vec);
}

/*void ModelMatrix::rotate() {}*/
