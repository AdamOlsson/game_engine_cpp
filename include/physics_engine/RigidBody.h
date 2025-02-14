#pragma once

#include "Coordinates.h"
#include "glm/fwd.hpp"
#include "shape.h"
#include <glm/glm.hpp>
#include <vector>

struct RigidBody {
    WorldPoint position;
    WorldPoint prev_position;
    Shape shape;
    float rotation = 0.0f;
    glm::vec3 velocity = glm::vec3(0.0, 0.0, 0.0);
    float angular_velocity = 0.0f;

    float mass = 1.0f;
    float inertia = 0.0f;
    float collision_restitution = 0.0f;

    std::vector<glm::vec3> vertices() const;
    std::vector<glm::vec3> normals() const;
    std::vector<glm::vec3> edges() const;

    bool is_point_inside(const WorldPoint &) const;
    WorldPoint closest_point_on_body(const WorldPoint &) const;
};

std::ostream &operator<<(std::ostream &os, const RigidBody &b);

class RigidBodyBuilder {
  private:
    std::optional<WorldPoint> position_;
    bool position_has_value_ = false;
    std::optional<Shape> shape_;
    bool shape_has_value_ = false;

    float rotation_ = 0.0f;
    WorldPoint prev_position_ = WorldPoint(0.0, 0.0, 0.0);
    glm::vec3 velocity_ = glm::vec3(0.0, 0.0, 0.0);
    float angular_velocity_ = 0.0f;
    float mass_ = 1.0f;
    float inertia_ = 1.0f;
    float collision_restitution_ = 1.0f;

  public:
    RigidBodyBuilder() = default;

    RigidBodyBuilder &position(const WorldPoint &pos) {
        position_ = pos;
        return *this;
    }

    RigidBodyBuilder &shape(const Shape &s) {
        shape_ = s;
        return *this;
    }

    // Optional parameters with default values
    RigidBodyBuilder &prev_position(const WorldPoint &pos) {
        prev_position_ = pos;
        return *this;
    }

    RigidBodyBuilder &rotation(float rot) {
        rotation_ = rot;
        return *this;
    }

    RigidBodyBuilder &velocity(const glm::vec3 &vel) {
        velocity_ = vel;
        return *this;
    }

    RigidBodyBuilder &angular_velocity(float ang_vel) {
        angular_velocity_ = ang_vel;
        return *this;
    }

    RigidBodyBuilder &mass(float m) {
        mass_ = m;
        return *this;
    }

    RigidBodyBuilder &inertia(float i) {
        inertia_ = i;
        return *this;
    }

    RigidBodyBuilder &collision_restitution(float c_r) {
        collision_restitution_ = c_r;
        return *this;
    }

    RigidBody build() const {
        if (!position_.has_value()) {
            throw std::runtime_error("Position must be set");
        }
        if (!shape_.has_value()) {
            throw std::runtime_error("Shape must be set");
        }

        return RigidBody{
            .position = position_.value(),
            .prev_position = WorldPoint(position_.value() - velocity_),
            .shape = shape_.value(),
            .rotation = rotation_,
            .velocity = velocity_,
            .angular_velocity = angular_velocity_,
            .mass = mass_,
            .inertia = inertia_,
            .collision_restitution = collision_restitution_,
        };
    }
};
