#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class IndexBuffer {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;
    bool cleanup_done;

  public:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;

    IndexBuffer(std::shared_ptr<CoreGraphicsContext> ctx, VkBuffer &buffer,
                VkDeviceMemory &bufferMemory)
        : buffer(buffer), bufferMemory(bufferMemory), ctx(ctx) {};

    ~IndexBuffer();

    void cleanup();
};

std::unique_ptr<IndexBuffer> createIndexBuffer(std::shared_ptr<CoreGraphicsContext> &ctx,
                                               const std::vector<uint16_t> &indices,
                                               VkCommandPool &commandPool,
                                               VkQueue &graphicsQueue);
