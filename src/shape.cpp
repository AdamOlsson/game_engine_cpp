#include "Shape.h"
#include <stdexcept>

Shape Shape::create_triangle_data(float side) { return Shape{Triangle{side}}; }

Shape Shape::create_rectangle_data(float width, float height) {
    return Shape{Rectangle{width, height}};
}

uint32_t Shape::encode_shape_type() const {
    return std::visit(
        [this](auto &&arg) -> uint32_t {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Triangle>) {
                return ShapeTypeEncoding::TriangleShape;
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return ShapeTypeEncoding::RectangleShape;
            } else {
                throw std::runtime_error("Shape not implemented (encode_shape_type)");
            }
        },
        params);
}
