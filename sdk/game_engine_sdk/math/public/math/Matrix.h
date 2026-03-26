#pragma once
#include "math/Vector2.h"
#include "traits.h"
#include "util/assert.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

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

    template <StaticCastableToFloat T> Matrix &translate(const T x, const T y) {
        m_matrix = glm::translate(m_matrix, glm::vec3(x, y, 0.0f));
        return *this;
    }

    Matrix &translate(const glm::vec2 &vec);
    Matrix &translate(const glm::vec3 &vec);

    Matrix &scale(const float x, const float y, const float z);
    Matrix &scale(const float x, const float y);
    Matrix &scale(const math::Vector2 &vec);
    Matrix &scale(const math::Vector2 &&vec);
    Matrix &scale(const glm::vec3 &vec);
    Matrix &scale(const glm::vec3 &&vec);
    template <StaticCastableToFloat T> Matrix &scale(const T value) {
        m_matrix = glm::scale(m_matrix, glm::vec3(static_cast<float>(value)));
        return *this;
    }

    Matrix &rotate(const float radians, const glm::vec3 &axis);
    Matrix &rotate_z(const float radians);

    operator glm::mat4() const { return m_matrix; }

    math::Vector2 position_2d() const {
        return math::Vector2(m_matrix[3].x, m_matrix[3].y);
    }
    static constexpr math::Vector2 position_2d(const glm::mat4 &mat) {
        return math::Vector2(mat[3].x, mat[3].y);
    }

    glm::vec3 position_3d() const {
        return glm::vec3(m_matrix[3].x, m_matrix[3].y, m_matrix[3].z);
    }
    static constexpr glm::vec3 position_3d(const glm::mat4 &mat) {
        return glm::vec3(mat[3].x, mat[3].y, mat[3].z);
    }

    static constexpr math::Vector2 scale_2d(const glm::mat4 &mat) {
        return math::Vector2(glm::length(glm::vec3(mat[0])),
                             glm::length(glm::vec3(mat[1])));
    }

    static constexpr math::Vector2 scale_2d_axis(const glm::mat4 &mat) {
        DEBUG_ASSERT(
            mat[0][1] == 0.0f && mat[1][0] == 0.0f,
            "Error: scale_2d_axis is only valid when the matrix is not rotated.");
        return math::Vector2(mat[0][0], mat[1][1]);
    }

    float rotation_z() const { return glm::atan(m_matrix[1].x, m_matrix[0].x); }
};
} // namespace math
