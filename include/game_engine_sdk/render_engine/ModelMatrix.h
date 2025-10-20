#pragma once
#include "glm/glm.hpp"

class ModelMatrix {
  private:
    glm::mat4 m_matrix;

  public:
    ModelMatrix();
    ModelMatrix(const glm::mat4 &mat);
    ~ModelMatrix() = default;

    /*void resize();*/

    void translate(const glm::vec3 &vec);
    /*void rotate();*/
};
