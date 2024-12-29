#pragma once

#include "vulkan/vulkan_core.h"
#include <memory>

struct IndexBuffer {
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;

    IndexBuffer(VkBuffer &buffer, VkDeviceMemory &bufferMemory)
        : buffer(buffer), bufferMemory(bufferMemory) {};
};

std::unique_ptr<IndexBuffer> createIndexBuffer(VkPhysicalDevice &physicalDevice,
                                               VkDevice &device,
                                               const std::vector<uint16_t> &indices,
                                               VkCommandPool &commandPool,
                                               VkQueue &graphicsQueue);
