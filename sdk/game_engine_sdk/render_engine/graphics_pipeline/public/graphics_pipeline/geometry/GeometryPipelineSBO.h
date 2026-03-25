#pragma once

#include "logger/io.h"
#include "util/colors.h"
#include <glm/glm.hpp>
#include <limits>
#include <sstream>

namespace graphics_pipeline::geometry {

class GeometrySBOHandle;

enum class GeometryShape : uint32_t {
    Rectangle = 0x0,
    Circle = 0x1,
    Triangle = 0x2,
};

struct GeometryPipelineSBO {
    alignas(16) glm::mat4 model_matrix = glm::mat4(1.0f);
    alignas(16) glm::vec4 color = util::colors::TRANSPARENT;
    struct {
        alignas(16) glm::vec4 color = util::colors::TRANSPARENT;
        alignas(4) float width = 0.0f;
        alignas(4) float radius = 0.0f;
    } border;
    alignas(4) uint32_t flags = 0;

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

class GeometrySBOHandle {
  private:
    friend class GeometryRenderer;
    friend class GeometryRenderer2;
    size_t id = std::numeric_limits<size_t>::max();

  public:
    GeometrySBOHandle() = default;
    GeometrySBOHandle(size_t id) : id(id) {}

    GeometrySBOHandle(const GeometrySBOHandle &) = delete;
    GeometrySBOHandle(GeometrySBOHandle &&other) noexcept = default;

    GeometrySBOHandle &operator=(const GeometrySBOHandle &&) = delete;
    GeometrySBOHandle &operator=(GeometrySBOHandle &&other) noexcept = default;
};

} // namespace graphics_pipeline::geometry
