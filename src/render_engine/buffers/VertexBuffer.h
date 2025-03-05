#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/shapes/Vertex.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class VertexBuffer {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;
    bool cleanup_done;

  public:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;

    VertexBuffer(std::shared_ptr<CoreGraphicsContext> ctx, VkBuffer &buffer,
                 VkDeviceMemory &bufferMemory)
        : buffer(buffer), bufferMemory(bufferMemory), ctx(ctx) {};

    ~VertexBuffer();

    void cleanup();
};

std::unique_ptr<VertexBuffer>
createVertexBuffer(std::shared_ptr<CoreGraphicsContext> &ctx,
                   const std::vector<Vertex> &vertices, VkCommandPool &commandPool,
                   VkQueue &graphicsQueue);
