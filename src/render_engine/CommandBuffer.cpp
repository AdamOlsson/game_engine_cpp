#include "CommandBuffer.h"
#include <stdexcept>

CommandBuffer::CommandBuffer(VkCommandBuffer &command_buffer)
    : command_buffer(command_buffer) {}

VkCommandBuffer CommandBuffer::begin() {
    vkResetCommandBuffer(command_buffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;                  // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional
    if (vkBeginCommandBuffer(command_buffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    return command_buffer;
}

void CommandBuffer::end() {
    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}
