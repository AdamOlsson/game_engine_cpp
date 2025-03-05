#pragma once
#include "io.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <variant>

struct Circle {
    alignas(4) glm::float32 radius;
};

struct Triangle {
    alignas(4) glm::float32 side;
};

struct Rectangle {
    alignas(4) glm::float32 width;
    alignas(4) glm::float32 height;
};

struct Hexagon {
    alignas(4) glm::float32 width;
};

// TODO: This shape is very large compared to structs only having one member. It is
// a lot of wasted space when we need to allocate memory after this struct. I should
// refactor this to keep separate buffers for each instance data type.
/*struct Arrow {*/
/*    alignas(16) glm::vec3 start;*/
/*    alignas(16) glm::vec3 stop;*/
/*    alignas(4) glm::float32 width;*/
/*};*/

/*struct Line {*/
/*    alignas(16) glm::vec3 start;*/
/*    alignas(16) glm::vec3 stop;*/
/*    alignas(4) glm::float32 width;*/
/*};*/

enum ShapeTypeEncoding {
    None = 0,
    CircleShape = 1,
    TriangleShape = 2,
    RectangleShape = 3,
    HexagonShape = 4,
    ArrowShape = 5,
    LineShape = 6,
};

struct Shape {
    std::variant<std::monostate, Circle, Triangle, Rectangle, Hexagon> params;

    template <typename T> bool is() const { return std::holds_alternative<T>(params); }
    template <typename T> T get() const { return std::get<T>(params); }

    static Shape create_circle_data(float radius);
    static Shape create_triangle_data(float side);
    static Shape create_rectangle_data(float width, float height);
    static Shape create_hexagon_data(float height);
    /*static Shape create_arrow_data(glm::vec3 &&start, glm::vec3 &&stop, float width);*/
    /*static Shape create_line_data(glm::vec3 &&start, glm::vec3 &&stop, float width);*/

    uint32_t encode_shape_type() const;
};

std::ostream &operator<<(std::ostream &os, const Shape &shape);
