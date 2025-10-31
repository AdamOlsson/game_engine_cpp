#pragma once

#include "game_engine_sdk/render_engine/CommandBufferManager.h"
#include "game_engine_sdk/render_engine/vulkan/context/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class IndexBuffer {
  private:
    std::shared_ptr<vulkan::context::GraphicsContext> m_ctx;

  public:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkDeviceSize size;
    size_t num_indices;

    IndexBuffer();
    IndexBuffer(std::shared_ptr<vulkan::context::GraphicsContext> ctx,
                const std::vector<uint16_t> &indices,
                CommandBufferManager *command_buffer_manager);
    ~IndexBuffer();

    IndexBuffer(IndexBuffer &&other) noexcept;
    IndexBuffer &operator=(IndexBuffer &&other) noexcept;
    IndexBuffer(const IndexBuffer &other) = delete;
    IndexBuffer &operator=(const IndexBuffer &other) = delete;
};
