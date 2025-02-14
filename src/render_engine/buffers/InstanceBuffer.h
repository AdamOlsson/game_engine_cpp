#pragma once

#include "vulkan/vulkan_core.h"
#include <sys/types.h>
#include <vector>

struct InstanceBuffer {
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;

    InstanceBuffer(VkBuffer &buffer, VkDeviceMemory &bufferMemory)
        : buffer(buffer), bufferMemory(bufferMemory) {};
};

std::unique_ptr<InstanceBuffer> createInstanceBuffer(VkPhysicalDevice &physicalDevice,
                                                     VkDevice &device,
                                                     const std::vector<uint> &vertices,
                                                     VkCommandPool &commandPool,
                                                     VkQueue &graphicsQueue);
