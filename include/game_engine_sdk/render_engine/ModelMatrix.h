#pragma once
#include "game_engine_sdk/traits.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

class ModelMatrix {
  private:
    glm::mat4 m_matrix;

  public:
    ModelMatrix();
    ModelMatrix(const glm::mat4 &mat);
    ~ModelMatrix() = default;

    ModelMatrix(const ModelMatrix &other);

    template <StaticCastableToFloat T>
    ModelMatrix &translate(const T x, const T y, const T z) {
        m_matrix = glm::translate(m_matrix, glm::vec3(x, y, z));
        return *this;
    }

    ModelMatrix &translate(const glm::vec3 &vec);

    ModelMatrix &scale(const float x, const float y, const float z);
    ModelMatrix &scale(const glm::vec3 &vec);
    ModelMatrix &scale(const glm::vec3 &&vec);
    template <StaticCastableToFloat T> ModelMatrix &scale(const T value) {
        m_matrix = glm::scale(m_matrix, glm::vec3(static_cast<float>(value)));
        return *this;
    }

    ModelMatrix &rotate(const float angle, const glm::vec3 &axis);

    operator glm::mat4() const { return m_matrix; }
};
