#pragma once

#include "render_engine/shapes/Vertex.h"
#include "vulkan/vulkan_core.h"

class VertexBuffer {
  public:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;

    VertexBuffer(VkDevice &device, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
        : buffer(buffer), bufferMemory(bufferMemory), device(&device) {};

    ~VertexBuffer();

    void cleanup();

  private:
    // TODO: Shared ptr
    VkDevice *device;
    bool cleanup_done;
};

std::unique_ptr<VertexBuffer> createVertexBuffer(VkPhysicalDevice &physicalDevice,
                                                 VkDevice &device,
                                                 const std::vector<Vertex> &vertices,
                                                 VkCommandPool &commandPool,
                                                 VkQueue &graphicsQueue);
