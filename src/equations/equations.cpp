#include "equations/equations.h"
#include <glm/glm.hpp>

float Equations::length2(const glm::vec3 &v) { return v.x * v.x + v.y * v.y + v.z * v.z; }

float Equations::cross_2d(const glm::vec3 &v1, const glm::vec3 &v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

glm::vec3 Equations::counterclockwise_perp_z(const glm::vec3 &v) {
    return glm::vec3(-v.y, v.x, 0.0f);
}

void Equations::counterclockwise_perp_z_mut(glm::vec3 &v) {
    float x = v.x;
    v.x = -v.y;
    v.y = x;
}

glm::vec3 Equations::clockwise_perp_z(const glm::vec3 &v) {
    return glm::vec3(v.y, -v.x, 0.0f);
}

void Equations::clockwise_perp_z_mut(glm::vec3 &v) {
    float x = v.x;
    v.x = v.y;
    v.y = -x;
}
glm::vec3 Equations::rotate_z(const glm::vec3 &point, float angle) {
    glm::mat3 rotation_matrix = {glm::vec3(cos(angle), -sin(angle), 0.0f),
                                 glm::vec3(sin(angle), cos(angle), 0.0f),
                                 glm::vec3(0.0f, 0.0f, 1.0f)};
    return rotation_matrix * point;
}

void Equations::rotate_z_mut(glm::vec3 &point, float angle) {
    glm::mat3 rotation_matrix = {glm::vec3(cos(angle), -sin(angle), 0.0f),
                                 glm::vec3(sin(angle), cos(angle), 0.0f),
                                 glm::vec3(0.0f, 0.0f, 1.0f)};
    point = rotation_matrix * point;
}

glm::vec2 Equations::rotate(const glm::vec2 &point, float angle) {
    glm::mat2 rotation_matrix = {glm::vec3(cos(angle), -sin(angle), 0.0f),
                                 glm::vec3(sin(angle), cos(angle), 0.0f)};
    return rotation_matrix * point;
}

void Equations::rotate_mut(glm::vec2 &point, float angle) {
    glm::mat2 rotation_matrix = {glm::vec3(cos(angle), -sin(angle), 0.0f),
                                 glm::vec3(sin(angle), cos(angle), 0.0f)};
    point = rotation_matrix * point;
}
