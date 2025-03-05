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
    VkDeviceSize size;
    size_t num_indices;

    IndexBuffer(std::shared_ptr<CoreGraphicsContext> ctx, VkBuffer &buffer,
                VkDeviceMemory &bufferMemory, VkDeviceSize size)
        : buffer(buffer), bufferMemory(bufferMemory), ctx(ctx), size(size),
          num_indices(size / sizeof(uint16_t)) {};

    ~IndexBuffer();

    void cleanup();
};

std::unique_ptr<IndexBuffer> createIndexBuffer(std::shared_ptr<CoreGraphicsContext> &ctx,
                                               const std::vector<uint16_t> &indices,
                                               const VkCommandPool &commandPool,
                                               const VkQueue &graphicsQueue);
