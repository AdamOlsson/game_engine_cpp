#include "CommandBufferManager.h"
#include "render_engine/SingleTimeCommandBuffer.h"
#include "vulkan/vulkan_core.h"

CommandBufferManager::CommandBufferManager(
    std::shared_ptr<graphics_context::GraphicsContext> ctx, const size_t num_buffers)
    : m_ctx(ctx), m_num_buffers(num_buffers), m_command_pool(create_command_pool()),
      m_command_buffers(create_command_buffers()) {}

CommandBufferManager::~CommandBufferManager() {
    if (m_ctx == nullptr) {
        return;
    }

    vkDestroyCommandPool(m_ctx->logical_device, m_command_pool, nullptr);
}

CommandBuffer CommandBufferManager::get_command_buffer() {
    CommandBuffer buf = CommandBuffer(m_command_buffers[m_next_buffer]);
    m_next_buffer = ++m_next_buffer % m_num_buffers;
    return buf;
}

SingleTimeCommandBuffer CommandBufferManager::get_single_time_command_buffer() {
    return SingleTimeCommandBuffer(m_ctx, m_command_pool);
}

VkCommandPool CommandBufferManager::create_command_pool() {
    graphics_context::device::QueueFamilyIndices queueFamilyIndices =
        m_ctx->physical_device.find_queue_families(m_ctx->surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    VkCommandPool commandPool;
    if (vkCreateCommandPool(m_ctx->logical_device, &poolInfo, nullptr, &commandPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
    return commandPool;
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
