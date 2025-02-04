#include "physics_engine/RigidBody.h"
#include "Coordinates.h"
#include "io.h"
#include "rectangle_equations.h"
#include "triangle_equations.h"
#include <ostream>

std::vector<glm::vec3> RigidBody::edges() const {
    return std::visit(
        [this](auto &&arg) -> std::vector<glm::vec3> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Triangle>) {
                return get_triangle_edges(*this);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return get_rectangle_edges(*this);
            } else {
                throw std::runtime_error("Shape not implemented (edges())");
            }
        },
        shape.params);
}

std::vector<glm::vec3> RigidBody::vertices() const {
    return std::visit(
        [this](auto &&arg) -> std::vector<glm::vec3> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Triangle>) {
                return get_triangle_vertices(*this);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return get_rectangle_vertices(*this);
            } else {
                throw std::runtime_error("Shape not implemented (vertices())");
            }
        },
        shape.params);
}

std::vector<glm::vec3> RigidBody::normals() const {
    return std::visit(
        [this](auto &&arg) -> std::vector<glm::vec3> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Triangle>) {
                return get_triangle_normals(*this);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return get_rectangle_normals(*this);
            } else {
                throw std::runtime_error("Shape not implemented (normals())");
            }
        },
        shape.params);
}

bool RigidBody::is_point_inside(const WorldPoint &point) const {
    return std::visit(
        [this, &point](auto &&arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Triangle>) {
                return is_point_inside_triangle(*this, point);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return is_point_inside_rectangle(*this, point);
            } else {
                throw std::runtime_error("Shape not implemented (is_point_inside())");
            }
        },
        shape.params);
}

WorldPoint RigidBody::closest_point_on_body(const WorldPoint &point) const {
    return std::visit(
        [this, &point](auto &&arg) -> WorldPoint {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Triangle>) {
                return closest_point_on_triangle(*this, point);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return closest_point_on_rectangle(*this, point);
            } else {
                throw std::runtime_error(
                    "Shape not implemented (closest_point_on_body())");
            }
        },
        shape.params);
}

std::ostream &operator<<(std::ostream &os, const RigidBody &b) {
    os << "RigidBody( position: " << b.position << ", prev_position: " << b.prev_position
       << ", velocity: " << b.velocity << ", angular_velocity: " << b.angular_velocity
       << ", rotation: " << b.rotation << ", mass: " << b.mass
       << ", inertia: " << b.inertia << ", shape: " << b.shape << ")";
    return os;
}
