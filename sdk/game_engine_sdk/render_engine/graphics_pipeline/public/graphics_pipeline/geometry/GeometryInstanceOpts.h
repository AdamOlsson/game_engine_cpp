#pragma once

#include "logger/io.h"
#include "util/colors.h"
#include <glm/glm.hpp>
#include <sstream>

namespace graphics_pipeline::geometry {

struct GeometryInstanceOpts {
    glm::mat4 model_matrix = glm::mat4(1.0f);
    glm::vec4 color = util::colors::TRANSPARENT;
    struct {
        glm::vec4 color = util::colors::TRANSPARENT;
        float width = 0.0f;
        float radius = 0.0f;
    } border;
    uint32_t flags = 0;

    std::string to_string() const {
        std::ostringstream oss;
        oss << "GeometryInstanceOpts {\n"
            << "  model_matrix: " << model_matrix << "\n"
            << "  color: " << color << "\n"
            << "  border: { " << "\n"
            << "    color: " << color << "\n"
            << "    width: " << border.width << "\n"
            << "    radius: " << border.radius << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const GeometryInstanceOpts &obj) {
        return os << obj.to_string();
    }
};

} // namespace graphics_pipeline::geometry
