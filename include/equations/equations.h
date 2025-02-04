#pragma once

#include "glm_config.h"

struct Equations {

    static float length2(const glm::vec3 &v);
    static float cross_2d(const glm::vec3 &v1, const glm::vec3 &v2);

    static glm::vec3 counterclockwise_perp_z(const glm::vec3 &);
    static void counterclockwise_perp_z_mut(glm::vec3 &);

    static glm::vec3 clockwise_perp_z(const glm::vec3 &);
    static void clockwise_perp_z_mut(glm::vec3 &);

    static glm::vec3 rotate_z(const glm::vec3 &point, float angle);
    static void rotate_z_mut(glm::vec3 &point, float angle);

    static glm::vec2 rotate(const glm::vec2 &point, float angle);
    static void rotate_mut(glm::vec2 &point, float angle);
};
