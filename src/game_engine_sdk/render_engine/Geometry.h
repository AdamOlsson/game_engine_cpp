#pragma once

#include "vulkan/Vertex.h"
#include <vector>

namespace Geometry {

const std::vector<uint16_t> quad_indices = {0, 1, 2, 0, 2, 3};
const std::vector<vulkan::Vertex> quad_vertices = {
    {-0.5f, -0.5f, 0.0f}, {-0.5f, 0.5f, 0.0f}, {0.5f, 0.5f, 0.0f}, {0.5f, -0.5f, 0.0f}};

} // namespace Geometry
