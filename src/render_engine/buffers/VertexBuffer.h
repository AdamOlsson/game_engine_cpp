#pragma once

#include "render_engine/shapes/Vertex.h"
#include "vulkan/vulkan_core.h"

struct VertexBuffer {
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;

    VertexBuffer(VkBuffer &buffer, VkDeviceMemory &bufferMemory)
        : buffer(buffer), bufferMemory(bufferMemory) {};
};

std::unique_ptr<VertexBuffer> createVertexBuffer(VkPhysicalDevice &physicalDevice,
                                                 VkDevice &device,
                                                 const std::vector<Vertex> &vertices,
                                                 VkCommandPool &commandPool,
                                                 VkQueue &graphicsQueue);
