#include "SingleTimeCommandBuffer.h"

SingleTimeCommandBuffer::SingleTimeCommandBuffer(std::shared_ptr<CoreGraphicsContext> ctx,
                                                 VkCommandPool &command_pool)
    : m_ctx(ctx), m_command_pool(command_pool), m_command_buffer(allocate_buffer()) {}

SingleTimeCommandBuffer::~SingleTimeCommandBuffer() {
    vkFreeCommandBuffers(m_ctx->device, m_command_pool, 1, &m_command_buffer);
}

VkCommandBuffer SingleTimeCommandBuffer::allocate_buffer() {
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    // You may wish to create a separate command pool for these kinds of
    // short-lived buffers, because the implementation may be able to apply
    // memory allocation optimizations. You should use the
    // VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag during command pool generation
    // in that cas
    alloc_info.commandPool = m_command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(m_ctx->device, &alloc_info, &command_buffer);
    return command_buffer;
}

void SingleTimeCommandBuffer::begin() {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(m_command_buffer, &begin_info);
}

void SingleTimeCommandBuffer::end() { vkEndCommandBuffer(m_command_buffer); }

void SingleTimeCommandBuffer::submit(const VkQueue &graphics_queue) {
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_command_buffer;

    vkQueueSubmit(graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphics_queue);
}
