#include "physics_engine/RigidBody.h"
#include "Coordinates.h"
#include "io.h"
#include "physics_engine/circle_equations.h"
#include "rectangle_equations.h"
#include "triangle_equations.h"
#include <ostream>
#include <sstream>
#include <stdexcept>

std::vector<glm::vec3> RigidBody::edges() const {
    return std::visit(
        [this](auto &&arg) -> std::vector<glm::vec3> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Circle>) {
                // We should not use the circle edges on physics calculations
                std::runtime_error(
                    "You are calling edges() on a circle rigid body. Are you "
                    "sure this is correct?");
            } else if constexpr (std::is_same_v<T, Triangle>) {
                return get_triangle_edges(*this);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return get_rectangle_edges(*this);
            }
            std::ostringstream oss;
            oss << "Shape::" << this->shape << " not implemented (edges())";
            throw std::runtime_error(oss.str());
        },
        shape.params);
}

std::vector<glm::vec3> RigidBody::vertices() const {
    return std::visit(
        [this](auto &&arg) -> std::vector<glm::vec3> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Circle>) {
                // We should not use the circle vertices on physics calculations
                std::runtime_error(
                    "You are calling vertices() on a circle rigid body. Are you "
                    "sure this is correct?");
            } else if constexpr (std::is_same_v<T, Triangle>) {
                return get_triangle_vertices(*this);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return get_rectangle_vertices(*this);
            }
            std::ostringstream oss;
            oss << "Shape::" << this->shape << " not implemented (vertices())";
            throw std::runtime_error(oss.str());
        },
        shape.params);
}

std::vector<glm::vec3> RigidBody::normals() const {
    return std::visit(
        [this](auto &&arg) -> std::vector<glm::vec3> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Circle>) {
                // We should not use the circle normals on physics calculations
                std::runtime_error(
                    "You are calling normals() on a circle rigid body. Are you "
                    "sure this is correct?");
            } else if constexpr (std::is_same_v<T, Triangle>) {
                return get_triangle_normals(*this);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return get_rectangle_normals(*this);
            }
            std::ostringstream oss;
            oss << "Shape::" << this->shape << " not implemented (normals())";
            throw std::runtime_error(oss.str());
        },
        shape.params);
}

float RigidBody::bounding_volume_radius() const {
    return std::visit(
        [this](auto &&arg) -> float {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Circle>) {
                return get_circle_bounding_volume_radius(*this);
            } else if constexpr (std::is_same_v<T, Triangle>) {
                return get_triangle_bounding_volume_radius(*this);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return get_rectangle_bounding_volume_radius(*this);
            }
            std::ostringstream oss;
            oss << "Shape::" << this->shape
                << " not implemented (bounding_volume_radius())";
            throw std::runtime_error(oss.str());
        },
        shape.params);
}

bool RigidBody::is_point_inside(const WorldPoint &point) const {
    return std::visit(
        [this, &point](auto &&arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Circle>) {
                return is_point_inside_circle(*this, point);
            } else if constexpr (std::is_same_v<T, Triangle>) {
                return is_point_inside_triangle(*this, point);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return is_point_inside_rectangle(*this, point);
            }
            std::ostringstream oss;
            oss << "Shape::" << this->shape << " not implemented (is_point_inside())";
            throw std::runtime_error(oss.str());
        },
        shape.params);
}

WorldPoint RigidBody::closest_point_on_body(const WorldPoint &point) const {
    return std::visit(
        [this, &point](auto &&arg) -> WorldPoint {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Circle>) {
                return closest_point_on_circle(*this, point);
            } else if constexpr (std::is_same_v<T, Triangle>) {
                return closest_point_on_triangle(*this, point);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return closest_point_on_rectangle(*this, point);
            }
            std::ostringstream oss;
            oss << "Shape::" << this->shape
                << " not implemented (closest_point_on_body())";
            throw std::runtime_error(oss.str());
        },
        shape.params);
}

float RigidBody::inertia() const {
    return std::visit(
        [this](auto &&arg) -> float {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, Circle>) {
                return circle_inertia(*this);
            } else if constexpr (std::is_same_v<T, Triangle>) {
                return triangle_inertia(*this);
            } else if constexpr (std::is_same_v<T, Rectangle>) {
                return rectangle_inertia(*this);
            }
            std::ostringstream oss;
            oss << "Shape::" << this->shape << " not implemented (inertia())";
            throw std::runtime_error(oss.str());
        },
        shape.params);
}

std::ostream &operator<<(std::ostream &os, const RigidBody &b) {
    os << "RigidBody( position: " << b.position << ", prev_position: " << b.prev_position
       << ", velocity: " << b.velocity << ", angular_velocity: " << b.angular_velocity
       << ", rotation: " << b.rotation << ", mass: " << b.mass
       << ", inertia: " << b.inertia() << ", shape: " << b.shape << ")";
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<RigidBody> &bs) {
    os << "[" << std::endl;
    for (auto b : bs) {
        os << "  " << b << ",\n";
    }
    os << "]";
    return os;
}
