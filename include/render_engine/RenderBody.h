#pragma once

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

struct RenderBody {
    glm::vec3 *position;
    glm::vec3 *color;
    glm::float32_t *rotation;

    RenderBody() = default;
    ~RenderBody() = default;
};
