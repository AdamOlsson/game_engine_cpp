#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <variant>

struct Triangle {
    alignas(4) glm::float32 side;
};

struct Rectangle {
    alignas(4) glm::float32 width;
    alignas(4) glm::float32 height;
};

enum ShapeTypeEncoding { None = 0, TriangleShape = 1, RectangleShape = 2 };

struct Shape {
    std::variant<std::monostate, Triangle, Rectangle> params;

    template <typename T> bool is() const { return std::holds_alternative<T>(params); }
    template <typename T> T get() const { return std::get<T>(params); }

    static Shape create_triangle_data(float side);
    static Shape create_rectangle_data(float width, float height);

    uint32_t encode_shape_type() const;
};
