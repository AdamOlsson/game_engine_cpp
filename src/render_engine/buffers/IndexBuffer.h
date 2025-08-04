#pragma once

#include "render_engine/CommandBufferManager.h"
#include "render_engine/graphics_context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class IndexBuffer {
  private:
    std::shared_ptr<graphics_context::GraphicsContext> m_ctx;

  public:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkDeviceSize size;
    size_t num_indices;

    IndexBuffer();
    IndexBuffer(std::shared_ptr<graphics_context::GraphicsContext> ctx,
                const std::vector<uint16_t> &indices,
                CommandBufferManager *command_buffer_manager);
    ~IndexBuffer();

    IndexBuffer &operator=(const IndexBuffer &) = delete;     // Copy assignment
    IndexBuffer &operator=(IndexBuffer &&) noexcept = delete; // Move assignment
    IndexBuffer(const IndexBuffer &) = delete;                // Copy
    IndexBuffer(IndexBuffer &&) noexcept = delete;            // Move
};
