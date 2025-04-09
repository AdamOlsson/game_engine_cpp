#pragma once

#include "render_engine/CoreGraphicsContext.h"
#include "vulkan/vulkan_core.h"
#include <memory>

class CommandBuffer {
  private:
    VkCommandBuffer command_buffer;

    // TODO: Understand these constructors and operations on more detail
    /*CommandBuffer &operator=(const CommandBuffer &); // Copy assignment*/
    /*CommandBuffer &operator=(CommandBuffer &&) noexcept; // Move assignment*/
    /*CommandBuffer(const CommandBuffer &); // Copy*/
    /*CommandBuffer(CommandBuffer &&) noexcept; // Move*/

  public:
    /*CommandBuffer() = default;*/
    CommandBuffer(VkCommandBuffer &command_buffer);

    VkCommandBuffer begin();
    void end();
};

class CommandHandler {
  private:
    std::shared_ptr<CoreGraphicsContext> ctx;
    VkCommandPool command_pool;

    size_t num_buffers;
    size_t next_buffer;
    std::vector<CommandBuffer> command_buffers;

    CommandHandler &operator=(const CommandHandler &);     // Copy assignment
    CommandHandler &operator=(CommandHandler &&) noexcept; // Move assignment
    CommandHandler(const CommandHandler &);                // Copy
    CommandHandler(CommandHandler &&) noexcept;            // Move

  public:
    CommandHandler(std::shared_ptr<CoreGraphicsContext> ctx, const size_t num_buffers);
    ~CommandHandler();

    VkCommandPool pool();
    CommandBuffer &buffer();
};
