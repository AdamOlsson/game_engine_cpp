#pragma once

#include "glm/fwd.hpp"
#include <glm/glm.hpp>

struct RigidBody {
    glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);
    glm::float32_t rotation = 0.0;

    RigidBody();
    RigidBody(glm::vec3 position, glm::float32_t rotation);
    ~RigidBody();
};
