#include "shape.h"

ShapeData create_triangle_data(float side) {
    return {.shape = Shape::Triangle, .param = {.triangle = {.side = side}}};
}
