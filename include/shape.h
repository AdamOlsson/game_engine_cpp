#pragma once

#include <glm/glm.hpp>

enum class Shape { None, Triangle, Square, Circle };

union ShapeParam {
    struct {
    } none;
    struct {
        glm::float32 side;
    } triangle;
    struct {
        glm::float32 width;
        glm::float32 height;
    } rectangle;
    struct {
        glm::float32 radius;
    } circle;
};

struct ShapeData {
    Shape shape;
    ShapeParam param;
};

ShapeData create_triangle_data(float side);
