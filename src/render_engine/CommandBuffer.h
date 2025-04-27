#pragma once

#include "vulkan/vulkan_core.h"

class CommandBuffer {
  private:
    VkCommandBuffer command_buffer;

  public:
    CommandBuffer(VkCommandBuffer &command_buffer);

    VkCommandBuffer begin();
    void end();
};
