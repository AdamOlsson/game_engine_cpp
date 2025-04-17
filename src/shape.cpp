#include "shape.h"
#include <ostream>
#include <stdexcept>

Shape Shape::create_circle_data(float diameter) { return Shape{Circle{diameter / 2.0f}}; }
Shape Shape::create_triangle_data(float side) { return Shape{Triangle{side}}; }
Shape Shape::create_rectangle_data(float width, float height) {
    return Shape{Rectangle{width, height}};
}
Shape Shape::create_hexagon_data(float width) { return Shape{Hexagon{width}}; }
/*Shape Shape::create_arrow_data(glm::vec3 &&start, glm::vec3 &&stop, float width) {*/
/*    return Shape{*/
/*        Arrow{.start = std::move(start), .stop = std::move(stop), .width = width}};*/
/*}*/
/*Shape Shape::create_line_data(glm::vec3 &&start, glm::vec3 &&stop, float width) {*/
/*    return Shape{*/
/*        Line{.start = std::move(start), .stop = std::move(stop), .width = width}};*/
/*}*/

uint32_t Shape::encode_shape_type() const {
    return std::visit(
        [this](auto &&arg) -> uint32_t {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Circle>) {
                return ShapeTypeEncoding::CircleShape;
            } else if constexpr (std::is_same_v<T, Triangle>) {
                return ShapeTypeEncoding::TriangleShape;
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return ShapeTypeEncoding::RectangleShape;
            } else if constexpr (std::is_same_v<T, Hexagon>) {
                return ShapeTypeEncoding::HexagonShape;
            }
            /*else if constexpr (std::is_same_v<T, Arrow>) {*/
            /*    return ShapeTypeEncoding::ArrowShape;*/
            /*} else if constexpr (std::is_same_v<T, Line>) {*/
            /*    return ShapeTypeEncoding::LineShape;*/
            /*}*/
            else {
                throw std::runtime_error("Shape not implemented (encode_shape_type)");
            }
        },
        params);
}

std::ostream &operator<<(std::ostream &os, const Shape &shape) {
    std::visit(
        [&os](const auto &param) {
            using T = std::decay_t<decltype(param)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                os << "None";
            } else if constexpr (std::is_same_v<T, Circle>) {
                os << "Circle(radius: " << param.radius << ")";
            } else if constexpr (std::is_same_v<T, Triangle>) {
                os << "Triangle(side: " << param.side << ")";
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                os << "Rectangle(width: " << param.width << ", height: " << param.height
                   << ")";
            } else if constexpr (std::is_same_v<T, Hexagon>) {
                os << "Hexagon(width: " << param.width << ")";
            }
            /*else if constexpr (std::is_same_v<T, Arrow>) {*/
            /*    os << "Arrow(start: " << param.start << ", stop: " << param.stop*/
            /*       << ", width: " << param.width << ")";*/
            /*} else if constexpr (std::is_same_v<T, Line>) {*/
            /*    os << "Line(start: " << param.start << ", stop: " << param.stop*/
            /*       << ", width: " << param.width << ")";*/
            /*}*/
        },

        shape.params);
    return os;
}
