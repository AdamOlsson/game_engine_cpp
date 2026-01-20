#pragma once

#include <iostream>
#include <vulkan/vulkan.h>

namespace vulkan {
struct alignas(16) Vertex {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    constexpr Vertex() {}
    constexpr Vertex(const float x, const float y, const float z) : x(x), y(y), z(z) {}

    static VkVertexInputBindingDescription get_binding_description() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 1> get_attribute_descriptions() {
        std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, x);

        return attributeDescriptions;
    }

    friend std::ostream &operator<<(std::ostream &os, const Vertex &v) {
        return os << "Vertex(" << v.x << ", " << v.y << ", " << v.z << ")";
    }

    friend std::ostream &operator<<(std::ostream &os, const std::vector<Vertex> &vs) {
        os << "[";
        for (auto v : vs) {
            os << v << ", ";
        }
        return os << "]";
    }
};
} // namespace vulkan
