#include "vulkan/CommandPool.h"
#include "vulkan/vulkan_core.h"

vulkan::CommandPool::CommandPool(std::shared_ptr<vulkan::context::GraphicsContext> ctx)
    : m_ctx(ctx), m_command_pool(create_command_pool()) {}

vulkan::CommandPool::~CommandPool() {
    if (m_command_pool == VK_NULL_HANDLE) {
        return;
    }
    vkDestroyCommandPool(m_ctx->logical_device, m_command_pool, nullptr);
}

vulkan::CommandPool::CommandPool(CommandPool &&other) noexcept
    : m_ctx(std::move(other.m_ctx)), m_command_pool(other.m_command_pool) {
    other.m_command_pool = VK_NULL_HANDLE;
}

vulkan::CommandPool &vulkan::CommandPool::operator=(CommandPool &&other) noexcept {
    if (this != &other) {
        if (m_command_pool != VK_NULL_HANDLE) {
            vkDestroyCommandPool(m_ctx->logical_device, m_command_pool, nullptr);
        }

        m_ctx = std::move(other.m_ctx);
        m_command_pool = other.m_command_pool;

        other.m_command_pool = VK_NULL_HANDLE;
    }
    return *this;
}

vulkan::SingleTimeCommandBuffer vulkan::CommandPool::get_single_time_command_buffer() {
    return SingleTimeCommandBuffer(m_ctx, m_command_pool);
}

VkCommandPool vulkan::CommandPool::create_command_pool() {
    context::device::QueueFamilyIndices queue_family_indices =
        m_ctx->physical_device.find_queue_families(m_ctx->surface);

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = queue_family_indices.graphicsFamily.value();

    VkCommandPool commandPool;
    if (vkCreateCommandPool(m_ctx->logical_device, &pool_info, nullptr, &commandPool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
    return commandPool;
}
