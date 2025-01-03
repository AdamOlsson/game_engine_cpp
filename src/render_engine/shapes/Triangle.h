
#include "render_engine/shapes/Vertex.h"
#include <cstdint>
#include <vector>
struct Triangle {
    static inline const std::vector<Vertex> vertices = {
        {{0.0f, -0.5f, 0.0f}, {-0.433f, 0.25f, 0.0f}, {0.433f, 0.25f, 0.0f}}};

    static inline const std::vector<uint16_t> indices = {0, 1, 2};
};