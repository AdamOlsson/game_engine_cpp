#pragma once

#include "util/colors.h"
#include "util/io.h"
#include <glm/glm.hpp>
#include <sstream>

namespace graphics_pipeline::geometry {
struct GeometryPipelineSBO {
    alignas(16) glm::mat4 model_matrix = glm::mat4(1.0f);
    alignas(16) glm::vec4 color = util::colors::TRANSPARENT;
    struct {
        alignas(16) glm::vec4 color = util::colors::TRANSPARENT;
        alignas(4) float width = 0.0f;
        alignas(4) float radius = 0.0f;
    } border;

    std::string to_string() const {
        std::ostringstream oss;
        oss << "GeometryPipelineSBO {\n"
            << "  model_matrix: " << model_matrix << "\n"
            << "  color: " << color << "\n"
            << "  border: { " << "\n"
            << "    color: " << color << "\n"
            << "    width: " << border.width << "\n"
            << "    radius: " << border.radius << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const GeometryPipelineSBO &obj) {
        return os << obj.to_string();
    }
};
} // namespace graphics_pipeline::geometry
