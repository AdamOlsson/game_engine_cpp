#pragma once

#include "game_engine_sdk/render_engine/SingleTimeCommandBuffer.h"
#include "game_engine_sdk/render_engine/vulkan/CommandBuffer.h"
#include "game_engine_sdk/render_engine/vulkan/CommandPool.h"
#include "game_engine_sdk/render_engine/vulkan/GraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class CommandBufferManager {
  private:
    std::shared_ptr<vulkan::GraphicsContext> m_ctx;
    vulkan::CommandPool m_command_pool;

    size_t m_num_buffers;
    size_t m_next_buffer;
    std::vector<VkCommandBuffer> m_command_buffers;

    std::vector<VkCommandBuffer> create_command_buffers();

  public:
    CommandBufferManager() = default;
    CommandBufferManager(std::shared_ptr<vulkan::GraphicsContext> ctx,
                         const size_t num_buffers);
    ~CommandBufferManager();

    CommandBufferManager &operator=(CommandBufferManager &&other) noexcept = default;
    CommandBufferManager(CommandBufferManager &&other) noexcept = default;

    CommandBufferManager &operator=(const CommandBufferManager &) = delete;
    CommandBufferManager(const CommandBufferManager &) = delete;

    vulkan::CommandBuffer get_command_buffer();
    SingleTimeCommandBuffer get_single_time_command_buffer();
};
