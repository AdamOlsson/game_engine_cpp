#pragma once
#include "graphics_pipeline/quad/QuadRenderer.h"
#include "logger/io.h"
#include "util/colors.h"
#include <glm/glm.hpp>
#include <sstream>

namespace graphics_pipeline::quad {

class QuadRenderer;

struct QuadPipelineSBO {
    alignas(16) glm::mat4 model_matrix = glm::mat4(1.0f);
    alignas(16) glm::vec4 uvwt = glm::vec4(-1.0f);
    alignas(4) glm::uint32 texture_id = 0;
    alignas(16) glm::vec4 color = util::colors::TRANSPARENT;

    std::string to_string() const {
        std::ostringstream oss;
        oss << "QuadPipelineUBO {\n"
            << "  model_matrix: " << model_matrix << "\n"
            << "  uvxt: " << uvwt << "\n"
            << "  texture_id: " << texture_id << "\n"
            << "  color: " << color << "\n"
            << "}";
        return oss.str();
    }

    friend std::ostream &operator<<(std::ostream &os, const QuadPipelineSBO &obj) {
        return os << obj.to_string();
    }
};

class QuadSBOHandle {
  private:
    friend class QuadRenderer;
    size_t id = std::numeric_limits<size_t>::max();

  public:
    QuadSBOHandle() = default;
    QuadSBOHandle(size_t id) : id(id) {}

    QuadSBOHandle(const QuadSBOHandle &) = delete;
    QuadSBOHandle(QuadSBOHandle &&other) noexcept = default;

    QuadSBOHandle &operator=(const QuadSBOHandle &&) = delete;
    QuadSBOHandle &operator=(QuadSBOHandle &&other) noexcept = default;
};

} // namespace graphics_pipeline::quad
