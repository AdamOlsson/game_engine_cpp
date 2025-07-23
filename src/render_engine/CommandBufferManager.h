#pragma once

#include "render_engine/CommandBuffer.h"
#include "render_engine/CoreGraphicsContext.h"
#include "render_engine/SingleTimeCommandBuffer.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class CommandBufferManager {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;
    VkCommandPool m_command_pool;

    size_t m_num_buffers;
    size_t m_next_buffer;
    std::vector<VkCommandBuffer> m_command_buffers;

    VkCommandPool create_command_pool();
    std::vector<VkCommandBuffer> create_command_buffers();

  public:
    CommandBufferManager() = default;
    CommandBufferManager(std::shared_ptr<CoreGraphicsContext> ctx,
                         const size_t num_buffers);
    ~CommandBufferManager();

    CommandBufferManager &operator=(CommandBufferManager &&other) noexcept = default;
    CommandBufferManager(CommandBufferManager &&other) noexcept = default;

    CommandBufferManager &operator=(const CommandBufferManager &) = delete;
    CommandBufferManager(const CommandBufferManager &) = delete;

    CommandBuffer get_command_buffer();
    SingleTimeCommandBuffer get_single_time_command_buffer();
};
