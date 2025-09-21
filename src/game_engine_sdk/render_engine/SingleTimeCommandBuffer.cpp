#include "game_engine_sdk/render_engine/SingleTimeCommandBuffer.h"
#include "vulkan/vulkan_core.h"

SingleTimeCommandBuffer::SingleTimeCommandBuffer(
    std::shared_ptr<graphics_context::GraphicsContext> ctx, VkCommandPool &command_pool)
    : m_ctx(ctx), m_command_pool(command_pool), m_command_buffer(allocate_buffer()) {}

SingleTimeCommandBuffer::~SingleTimeCommandBuffer() {
    vkFreeCommandBuffers(m_ctx->logical_device, m_command_pool, 1, &m_command_buffer);
}

void SingleTimeCommandBuffer::copy_buffer(const VkBuffer &src, const VkBuffer &dst,
                                          const VkBufferCopy &region) {
    vkCmdCopyBuffer(m_command_buffer, src, dst, 1, &region);
}

void SingleTimeCommandBuffer::copy_buffer_to_image(const VkBuffer &buffer,
                                                   const VkImage &image,
                                                   const VkImageLayout image_layout,
                                                   const VkBufferImageCopy &region) {
    vkCmdCopyBufferToImage(m_command_buffer, buffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void SingleTimeCommandBuffer::barrier(const VkPipelineStageFlagBits2 &source_stage,
                                      const VkPipelineStageFlagBits2 &destination_stage,
                                      const VkImageMemoryBarrier &barrier) {
    vkCmdPipelineBarrier(m_command_buffer, source_stage, destination_stage, 0, 0, nullptr,
                         0, nullptr, 1, &barrier);
}

void SingleTimeCommandBuffer::begin() {
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(m_command_buffer, &begin_info);
}

void SingleTimeCommandBuffer::end() { vkEndCommandBuffer(m_command_buffer); }

void SingleTimeCommandBuffer::submit() {
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_command_buffer;

    auto device_queues = m_ctx->get_device_queues();

    vkQueueSubmit(device_queues.graphics_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(device_queues.graphics_queue);
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
    vkAllocateCommandBuffers(m_ctx->logical_device, &alloc_info, &command_buffer);
    return command_buffer;
}
