#pragma once

#include "shape.h"
#include <stdexcept>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

struct RenderBody {
    glm::vec3 color;
    glm::vec3 position = glm::vec3(0.0, 0.0, 0.0);
    glm::float32 rotation = 0.0;
    Shape shape = Shape::create_rectangle_data(1.0, 1.0);
};

std::ostream &operator<<(std::ostream &os, const RenderBody &b);

class RenderBodyBuilder {
  private:
    // Required parameter with validity flag
    glm::vec3 color_;
    bool color_has_value_ = false;

    // Optional parameters with default values
    glm::vec3 position_ = glm::vec3(0.0, 0.0, 0.0);
    glm::float32 rotation_ = 0.0;
    Shape shape_ = Shape::create_rectangle_data(1.0, 1.0);

  public:
    RenderBodyBuilder() = default;

    RenderBodyBuilder &color(const glm::vec3 &c) {
        color_ = c;
        color_has_value_ = true;
        return *this;
    }

    RenderBodyBuilder &position(const glm::vec3 &pos) {
        position_ = pos;
        return *this;
    }

    RenderBodyBuilder &rotation(glm::float32 rot) {
        rotation_ = rot;
        return *this;
    }

    RenderBodyBuilder &shape(const Shape &s) {
        shape_ = s;
        return *this;
    }

    RenderBody build() const {
        if (!color_has_value_) {
            throw std::runtime_error("Color must be set");
        }

        RenderBody body;
        body.color = color_;
        body.position = position_;
        body.rotation = rotation_;
        body.shape = shape_;

        return body;
    }
};
