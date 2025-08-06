#include "CommandBufferManager.h"
#include "render_engine/SingleTimeCommandBuffer.h"
#include "vulkan/vulkan_core.h"

CommandBufferManager::CommandBufferManager(
    std::shared_ptr<graphics_context::GraphicsContext> ctx, const size_t num_buffers)
    : m_ctx(ctx), m_num_buffers(num_buffers), m_command_pool(vulkan::CommandPool(m_ctx)),
      m_command_buffers(create_command_buffers()) {}

CommandBufferManager::~CommandBufferManager() {}

vulkan::CommandBuffer CommandBufferManager::get_command_buffer() {
    vulkan::CommandBuffer buf = vulkan::CommandBuffer(m_command_buffers[m_next_buffer]);
    m_next_buffer = ++m_next_buffer % m_num_buffers;
    return buf;
}

SingleTimeCommandBuffer CommandBufferManager::get_single_time_command_buffer() {
    return m_command_pool.get_single_time_command_buffer();
}

std::vector<VkCommandBuffer> CommandBufferManager::create_command_buffers() {
    std::vector<VkCommandBuffer> command_buffers;
    command_buffers.resize(m_num_buffers);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_command_pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)command_buffers.size();

    if (vkAllocateCommandBuffers(m_ctx->logical_device, &allocInfo,
                                 command_buffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    return command_buffers;
}
