#pragma once

#include "shape.h"
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

struct RenderBody {
    glm::vec3 color;
    glm::vec3 position;
    glm::float32 rotation;
    ShapeData shape;
};
