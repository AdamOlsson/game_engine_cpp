#pragma once

#include <glm/glm.hpp>

struct RigidBody {
    glm::vec3 *position;
    glm::float32_t *rotation;

    RigidBody() = default;
    ~RigidBody() = default;
};
