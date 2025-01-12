#include "Shape.h"

ShapeData create_triangle_data(float side) {
    return {.shape = Shape::Triangle, .param = {.triangle = {.side = side}}};
}

ShapeData create_rectangle_data(float width, float height) {
    return {.shape = Shape::Rectangle,
            .param = {.rectangle = {.width = width, .height = height}}};
}
