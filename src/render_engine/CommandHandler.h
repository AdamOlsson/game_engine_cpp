#pragma once

#include "render_engine/CommandBuffer.h"
#include "render_engine/CoreGraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

typedef VkCommandPool CommandPool;

class CommandHandler {
  private:
    std::shared_ptr<CoreGraphicsContext> m_ctx;
    VkCommandPool m_command_pool;

    size_t m_num_buffers;
    size_t m_next_buffer;
    std::vector<CommandBuffer> m_command_buffers;

    CommandPool create_command_pool();
    std::vector<CommandBuffer> create_command_buffers();

  public:
    CommandHandler(std::shared_ptr<CoreGraphicsContext> ctx, const size_t num_buffers);
    ~CommandHandler();

    CommandHandler &operator=(const CommandHandler &) = delete;
    CommandHandler &operator=(CommandHandler &&) noexcept = delete;
    CommandHandler(const CommandHandler &) = delete;
    CommandHandler(CommandHandler &&) = delete;

    CommandPool pool();
    CommandBuffer &buffer();
};
