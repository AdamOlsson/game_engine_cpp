
#include "render_engine/shapes/Vertex.h"
#include <cstdint>
#include <vector>

struct Geometry {
    static inline const std::vector<uint16_t> triangle_indices = {0, 1, 2};
    static inline const std::vector<Vertex> triangle_vertices = {
        {{0.0f, -0.577f, 0.0f}, {-0.5f, 0.289f, 0.0f}, {0.5f, 0.289f, 0.0f}}};

    static inline const std::vector<uint16_t> rectangle_indices = {0, 1, 2, 0, 2, 3};
    static inline const std::vector<Vertex> rectangle_vertices = {{-0.5f, -0.5f, 0.0f},
                                                                  {-0.5f, 0.5f, 0.0f},
                                                                  {0.5f, 0.5f, 0.0f},
                                                                  {0.5f, -0.5f, 0.0f}};
};
