#pragma once

#include "vulkan/vulkan_core.h"

class CommandBuffer {
  private:
    VkCommandBuffer m_command_buffer;

  public:
    CommandBuffer() = default;
    CommandBuffer(VkCommandBuffer &command_buffer);
    ~CommandBuffer() = default;

    CommandBuffer(CommandBuffer &&other) noexcept = default;
    CommandBuffer &operator=(CommandBuffer &&other) noexcept = default;

    CommandBuffer(const CommandBuffer &other) = default;
    CommandBuffer &operator=(const CommandBuffer &other) = default;

    void begin();
    void end();

    operator VkCommandBuffer() const { return m_command_buffer; }
};
