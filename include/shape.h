#pragma once
#include <cstdint>
#include <format>
#include <glm/glm.hpp>
#include <stdexcept>
#include <string>
#include <variant>

struct Circle {
    alignas(4) glm::float32 diameter;
    alignas(4) glm::float32 height;

    // Pretty print member function
    std::string to_string() const {
        return std::format("Circle {{ diameter: {:.3f} }}", diameter);
    }

    // Stream operator
    friend std::ostream &operator<<(std::ostream &os, const Circle &circle) {
        return os << circle.to_string();
    }

    operator glm::vec2() const { return glm::vec2(diameter, height); }
};

struct Triangle {
    alignas(4) glm::float32 side;
    alignas(4) glm::float32 _dummy;

    std::string to_string() const {
        return std::format("Triangle {{ side: {:.3f} }}", side);
    }

    friend std::ostream &operator<<(std::ostream &os, const Triangle &triangle) {
        return os << triangle.to_string();
    }

    operator glm::vec2() const { return glm::vec2(side, _dummy); }
};

struct Rectangle {
    alignas(4) glm::float32 width;
    alignas(4) glm::float32 height;

    std::string to_string() const {
        return std::format("Rectangle {{ width: {:.3f}, height: {:.3f} }}", width,
                           height);
    }

    friend std::ostream &operator<<(std::ostream &os, const Rectangle &rect) {
        return os << rect.to_string();
    }

    operator glm::vec2() const { return glm::vec2(width, height); }
};

struct Hexagon {
    alignas(4) glm::float32 width;
    alignas(4) glm::float32 height;

    std::string to_string() const {
        return std::format("Hexagon {{ width: {:.3f}, height: {:.3f} }}", width, height);
    }

    friend std::ostream &operator<<(std::ostream &os, const Hexagon &hexagon) {
        return os << hexagon.to_string();
    }

    operator glm::vec2() const { return glm::vec2(width, height); }
};

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

    static Shape create_circle_data(float diameter);
    static Shape create_triangle_data(float side);
    static Shape create_rectangle_data(float width, float height);
    static Shape create_hexagon_data(float height);

    uint32_t encode_shape_type() const;

    std::string to_string() const {
        return std::visit(
            [](const auto &param) -> std::string {
                using T = std::decay_t<decltype(param)>;
                if constexpr (std::is_same_v<T, std::monostate>) {
                    return "Shape { None }";
                } else {
                    return std::format("Shape {{ {} }}", param.to_string());
                }
            },

            params);
    }

    friend std::ostream &operator<<(std::ostream &os, const Shape &shape) {
        return os << shape.to_string();
    }

    operator glm::vec2() const {
        return std::visit(
            [](const auto &param) -> glm::vec2 {
                using T = std::decay_t<decltype(param)>;
                if constexpr (std::is_same_v<T, std::monostate>) {
                    throw std::runtime_error("Assigning invalid shape");
                } else {
                    return param;
                }
            },

            params);
        ;
    };
};
