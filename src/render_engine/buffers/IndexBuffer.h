#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class IndexBuffer {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;

  public:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkDeviceSize size;
    size_t num_indices;

    IndexBuffer();
    IndexBuffer(std::shared_ptr<CoreGraphicsContext> ctx,
                const std::vector<uint16_t> &indices, const VkCommandPool &commandPool,
                const VkQueue &graphicsQueue);
    ~IndexBuffer();

    IndexBuffer &operator=(const IndexBuffer &) = delete;     // Copy assignment
    IndexBuffer &operator=(IndexBuffer &&) noexcept = delete; // Move assignment
    IndexBuffer(const IndexBuffer &) = delete;                // Copy
    IndexBuffer(IndexBuffer &&) noexcept = delete;            // Move
};
