#pragma once
#include "glm/glm.hpp"

class ModelMatrix {
  private:
    glm::mat4 m_matrix;

  public:
    ModelMatrix();
    ModelMatrix(const glm::mat4 &mat);
    ~ModelMatrix() = default;

    ModelMatrix(const ModelMatrix &other);

    ModelMatrix &scale(const float x, const float y, const float z);
    ModelMatrix &scale(const glm::vec3 &vec);
    ModelMatrix &scale(const glm::vec3 &&vec);

    ModelMatrix &translate(const float x, const float y, const float z);
    ModelMatrix &translate(const glm::vec3 &vec);

    ModelMatrix &rotate(const float angle, const glm::vec3 &axis);

    operator glm::mat4() const { return m_matrix; }
};
