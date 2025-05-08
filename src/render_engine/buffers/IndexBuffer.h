#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/SwapChainManager.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class IndexBuffer {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;

  public:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkDeviceSize size;
    size_t num_indices;

    IndexBuffer();
    IndexBuffer(std::shared_ptr<CoreGraphicsContext> ctx,
                const std::vector<uint16_t> &indices,
                SwapChainManager &swap_chain_manager);
    ~IndexBuffer();

    IndexBuffer &operator=(const IndexBuffer &) = delete;     // Copy assignment
    IndexBuffer &operator=(IndexBuffer &&) noexcept = delete; // Move assignment
    IndexBuffer(const IndexBuffer &) = delete;                // Copy
    IndexBuffer(IndexBuffer &&) noexcept = delete;            // Move
};
