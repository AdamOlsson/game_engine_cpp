#pragma once

#include "shape.h"
#include <glm/glm.hpp>

struct RigidBody {
    glm::vec3 position;
    glm::float32_t rotation;
    ShapeData shape_data;

    bool is_point_inside(glm::vec2 &);
};
