#pragma once

#include <iostream>
#include <vulkan/vulkan.h>

namespace graphics_pipeline::text {
struct alignas(16) GlyphVertex {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float u = 0.0f;
    float v = 0.0f;
    float w = 0.0f;

    constexpr GlyphVertex() {}
    constexpr GlyphVertex(const float x, const float y, const float z, const float u,
                          const float v, const float w)
        : x(x), y(y), z(z), u(u), v(v), w(w) {}

    constexpr GlyphVertex(const std::array<float, 3> xyz, const std::array<float, 3> uvw)
        : x(xyz[0]), y(xyz[1]), z(xyz[2]), u(uvw[0]), v(uvw[1]), w(uvw[2]) {}

    static VkVertexInputBindingDescription get_binding_description() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(GlyphVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> get_attribute_descriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(GlyphVertex, x);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(GlyphVertex, u);

        return attributeDescriptions;
    }

    friend std::ostream &operator<<(std::ostream &os, const GlyphVertex &v) {
        return os << std::format("GlyphVertex{{xyz:({},{},{}), uvw: ({},{},{})}}", v.x,
                                 v.y, v.z, v.u, v.v, v.w);
    }

    friend std::ostream &operator<<(std::ostream &os,
                                    const std::vector<GlyphVertex> &vs) {
        os << "[";
        for (auto v : vs) {
            os << v << ", ";
        }
        return os << "]";
    }
};
} // namespace graphics_pipeline::text
