#pragma once
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

template <typename T>
concept StaticCastableToFloat = requires(T t) {
    { static_cast<float>(t) };
};

namespace math {
class Matrix {
  private:
    glm::mat4 m_matrix;

  public:
    Matrix();
    Matrix(const glm::mat4 &mat);
    ~Matrix() = default;

    Matrix(const Matrix &other) = default;
    Matrix(Matrix &&other) noexcept = default;

    Matrix &operator=(const Matrix &other) = default;
    Matrix &operator=(Matrix &&other) noexcept = default;

    template <StaticCastableToFloat T>
    Matrix &translate(const T x, const T y, const T z) {
        m_matrix = glm::translate(m_matrix, glm::vec3(x, y, z));
        return *this;
    }

    Matrix &translate(const glm::vec2 &vec);
    Matrix &translate(const glm::vec3 &vec);

    Matrix &scale(const float x, const float y, const float z);
    Matrix &scale(const glm::vec3 &vec);
    Matrix &scale(const glm::vec3 &&vec);
    template <StaticCastableToFloat T> Matrix &scale(const T value) {
        m_matrix = glm::scale(m_matrix, glm::vec3(static_cast<float>(value)));
        return *this;
    }

    Matrix &rotate(const float angle, const glm::vec3 &axis);

    operator glm::mat4() const { return m_matrix; }

    glm::vec2 position_2d() const { return glm::vec2(m_matrix[3].x, m_matrix[3].y); }
    glm::vec3 position_3d() const {
        return glm::vec3(m_matrix[3].x, m_matrix[3].y, m_matrix[3].z);
    }
};
} // namespace math
