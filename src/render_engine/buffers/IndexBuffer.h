#pragma once

#include "vulkan/vulkan_core.h"
#include <memory>

class IndexBuffer {
  public:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;

    IndexBuffer(VkDevice &device, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
        : buffer(buffer), bufferMemory(bufferMemory), device(&device) {};

    ~IndexBuffer();

    void cleanup();

  private:
    // TODO: Shared ptr
    VkDevice *device;
    bool cleanup_done;
};

std::unique_ptr<IndexBuffer> createIndexBuffer(VkPhysicalDevice &physicalDevice,
                                               VkDevice &device,
                                               const std::vector<uint16_t> &indices,
                                               VkCommandPool &commandPool,
                                               VkQueue &graphicsQueue);
